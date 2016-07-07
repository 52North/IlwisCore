#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "connectorinterface.h"
#include "table.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "identifierrange.h"
#include "basetable.h"
#include "flattable.h"
#include "tablemerger.h"

using namespace Ilwis;

TableMerger::TableMerger()
{
}

bool TableMerger::copyColumns(const ITable &tblSource, ITable& tbltarget, int options) {
    int index = 0;
    if (!tblSource.isValid() || !tbltarget.isValid()){
        return ERROR1(ERR_NO_INITIALIZED_1, "Tables");
    }
    std::map<QString, QString> nameMapping;
    for(int c1 = 0; c1 < tblSource->columnCount(); ++c1) {
        auto coldef1 = tblSource->columndefinition(c1);
        ColumnDefinition coldef2;
        if ( (coldef2=tbltarget->columndefinition(coldef1.name())).isValid()){
            if (!coldef1.datadef().isCompatibleWith(coldef2.datadef()) || options != 0){
                ColumnDefinition coldef( QString("%1_%2").arg(tblSource->name()).arg(coldef1.name()), coldef1.datadef().domain<>(), tbltarget->columnCount() + index);
                tbltarget->addColumn(coldef);
                nameMapping[coldef1.name()] = coldef.name();
                continue;
            }
        }else {
            tbltarget->addColumn(coldef1);
        }
        nameMapping[coldef1.name()] = coldef1.name();
    }
    for(int col=0; col < tblSource->columnCount(); ++col ) {
        const auto& coldef = tblSource->columndefinition(col);
        auto values = tblSource->column(coldef.name()) ;
        QString colName = nameMapping[coldef.name()];
        tbltarget->column(colName,values);
    }
    return true;
}

bool TableMerger::mergeMetadataTables(ITable& tblOut, const ITable& tblIn, const std::vector<QString>& columns) {
    for(const auto colName : columns) {
        auto coldefIn = tblIn->columndefinition(colName);
        if ( !coldefIn.isValid()) {
            ERROR2(ERR_NOT_FOUND2, colName, tblIn->name());
            return false;
        }
        ColumnDefinition coldefOut;
        if ( (coldefOut=tblOut->columndefinition(coldefIn.name())).isValid()){
            if (coldefIn.datadef().isCompatibleWith(coldefOut.datadef())){
                ColumnDefinition defnew = mergeColumnDefinitions(coldefOut, coldefIn, &(_renumberers[colName]));
                tblOut->columndefinition(coldefOut.id()) = defnew;
            } else {
                ERROR2(ERR_NOT_COMPATIBLE2,"column definition",tblIn->name());
                return false;
            }
        }else {
            tblOut->addColumn(ColumnDefinition(coldefIn, tblOut->columnCount()));
        }
        quint32 recs = std::max(tblIn->recordCount(), tblOut->recordCount());
        tblOut->recordCount(recs);
    }
    return true;
}

ITable TableMerger::mergeMetadataTables(const ITable& tbl1, const ITable& tbl2) {
    if (!tbl1.isValid() || !tbl2.isValid())
        return ITable();

    std::vector<ColumnDefinition> newdefs;
    quint32 records = 0;
    quint32 index = 0;
    for(int c1 = 0; c1 < tbl1->columnCount(); ++c1) {
        auto coldef1 = tbl1->columndefinition(c1);
        if ( !coldef1.isValid()){
            WARN2(WARN_INVALID_OBJECT, TR("column"), tbl1->name());
            continue;
        }

        ColumnDefinition coldef2;
        if ( (coldef2=tbl2->columndefinition(coldef1.name())).isValid() && coldef1.name() != COVERAGEKEYCOLUMN){
            if (coldef1.datadef().isCompatibleWith(coldef2.datadef())){
                ColumnDefinition defnew = mergeColumnDefinitions(coldef1, coldef2, &(_renumberers[coldef2.name()]));
                newdefs.push_back(ColumnDefinition(defnew, index));
            } else {
                QString newname = tbl2->name() + "_" + coldef2.name();
                _columnRenames[coldef2.name()] = newname;
                newdefs.push_back(ColumnDefinition(coldef1, index));
                ColumnDefinition defnew(coldef2);
                defnew.name(newname);
                newdefs.push_back(ColumnDefinition(defnew, index));

            }
        }else {
            newdefs.push_back(ColumnDefinition(coldef1, index));
        }
        ++index;
    }
    for(int c2 = 0; c2 < tbl2->columnCount(); ++c2) {
        auto coldef2 = tbl2->columndefinition(c2);
        if ( !coldef2.isValid()) {
            WARN2(WARN_INVALID_OBJECT, TR("column"), tbl2->name());
            continue;
        }

        if ( coldef2.name() == COVERAGEKEYCOLUMN) {
            QString colName = COVERAGEKEYCOLUMN;
            if (tbl1->columndefinition(colName).isValid() ){
                colName =QString("%1_%2").arg(COVERAGEKEYCOLUMN).arg(tbl2->id());
                _columnRenames[coldef2.name()] = colName;
            }
            newdefs.push_back(ColumnDefinition(coldef2, index++));
            newdefs.back().name(colName);
        } else{
            if ( !tbl1->columndefinition(coldef2.name()).isValid()){
                newdefs.push_back(ColumnDefinition(coldef2, index++));
            }
        }
    }

    IFlatTable newTable;
    newTable.prepare(QString(INTERNAL_CATALOG + "/%1_%2").arg(tbl1->name(), tbl2->name()));
    newTable->recordCount(records);
    for(const ColumnDefinition& def : newdefs) {
        newTable->addColumn(def);
    }
    return newTable;
}

void TableMerger::mergeTableData(const ITable &sourceTable1,const ITable &sourceTable2, ITable &targetTable, const std::vector<QString>& except) const
{
    for(int col=0; col < sourceTable1->columnCount(); ++col ) {
        const auto& coldef = sourceTable1->columndefinition(col);
        auto iter = std::find(except.begin(), except.end(), coldef.name());
        if ( iter != except.end())
            continue;
        auto values = sourceTable1->column(coldef.name()) ;
        targetTable->column(coldef.name(),values);
    }
    for(int col=0; col < sourceTable2->columnCount(); ++col ) {
        const auto& coldef = sourceTable2->columndefinition(col);
        auto iterex = std::find(except.begin(), except.end(), coldef.name());
        if ( iterex != except.end())
            continue;
        auto values = sourceTable2->column(coldef.name()) ;
        QString targetColName = coldef.name();
        std::map<QString, QString>::const_iterator iter;
        if ( (iter = _columnRenames.find(targetColName)) != _columnRenames.end()) {
            targetColName = (*iter).second;
        }
        std::map<QString, RenumberMap>::const_iterator iterR;
        if ( (iterR = _renumberers.find(targetColName)) != _renumberers.end()) {
            const RenumberMap& renumberer = (*iterR).second;
            for(auto pair : renumberer) {
                for(auto& val : values) {
                    if ( pair.first == val)
                        val = pair.second;
                }
            }
        }
        targetTable->column(targetColName,values, sourceTable1->recordCount());
    }
}

ColumnDefinition TableMerger::mergeColumnDefinitions(const ColumnDefinition &def1, const ColumnDefinition &def2, RenumberMap *renumberer) {
    Range *rng = 0;

    if ( hasType(def1.datadef().domain<>()->valueType(),itNUMBER)){
        rng = NumericRange::merge(def1.datadef().range<NumericRange>(), def1.datadef().range<NumericRange>());
    } else if (def1.datadef().domain<>()->valueType() == itINDEXEDITEM ) {
        rng = IndexedIdentifierRange::merge(def1.datadef().range<IndexedIdentifierRange>(), def1.datadef().range<IndexedIdentifierRange>());
    } else if (def1.datadef().domain<>()->valueType() == itNAMEDITEM ) {
        rng = NamedIdentifierRange::merge(def1.datadef().range<NamedIdentifierRange>(), def1.datadef().range<NamedIdentifierRange>(), renumberer);
    } else if (def1.datadef().domain<>()->valueType() == itTHEMATICITEM ) {
        rng = ThematicRange::merge(def1.datadef().range<ThematicRange>(), def1.datadef().range<ThematicRange>(), renumberer);
    }

    QString colname = def1.name() + "_" + def2.name();
    ColumnDefinition coldef(colname, def1.datadef().domain<>());
    if ( rng)
        coldef.datadef().range(rng);

    return coldef;
}



