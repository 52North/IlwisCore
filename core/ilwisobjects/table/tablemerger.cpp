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

ITable TableMerger::mergeTables(const ITable& tbl1, const ITable& tbl2, RenumberMap *renumberer ) {
    std::vector<ColumnDefinition> newdefs;
    quint32 records = 0;
    for(int c1 = 0; c1 < tbl1->columns(); ++c1) {
        auto coldef1 = tbl1->columndefinition(c1);
        ColumnDefinition coldef2;
        if ( (coldef2=tbl2->columndefinition(coldef1.name())).isValid() && coldef1.name() != COVERAGEKEYCOLUMN){
            if (coldef1.datadef().isCompatibleWith(coldef2.datadef())){
                ColumnDefinition defnew = mergeColumnDefinitions(coldef1, coldef2, renumberer);
                newdefs.push_back(ColumnDefinition(defnew));
            } else {
                QString newname = tbl2->name() + "_" + coldef2.name();
                newdefs.push_back(ColumnDefinition(coldef1));
                ColumnDefinition defnew(coldef2);
                defnew.setName(newname);
                newdefs.push_back(ColumnDefinition(defnew));

            }
        }else {
            newdefs.push_back(ColumnDefinition(coldef1));
        }
    }
    for(int c2 = 0; c2 < tbl2->columns(); ++c2) {
        auto coldef2 = tbl2->columndefinition(c2);
        if ( coldef2.name() == COVERAGEKEYCOLUMN) {
            QString colName = COVERAGEKEYCOLUMN;
            if (tbl1->columndefinition(COVERAGEKEYCOLUMN).isValid() ){
                colName =QString("%1_%2").arg(COVERAGEKEYCOLUMN).arg(tbl2->id());
            }
            newdefs.push_back(ColumnDefinition(coldef2));
            newdefs.back().setName(colName);
        } else{
            if ( !tbl1->columndefinition(coldef2.name()).isValid()){
                newdefs.push_back(ColumnDefinition(coldef2));
            }
        }
    }

    IFlatTable newTable;
    newTable.prepare();
    newTable->records(records);
    for(const ColumnDefinition& def : newdefs) {
        newTable->addColumn(def);
    }
    return newTable;
}

ColumnDefinition TableMerger::mergeColumnDefinitions(const ColumnDefinition &def1, const ColumnDefinition &def2, RenumberMap *renumberer) {
    Range *rng = 0;

    if ( hasType(def1.datadef().domain()->valueType(),itNUMBER)){
        rng = NumericRange::merge(def1.datadef().range<NumericRange>(), def1.datadef().range<NumericRange>());
    } else if (def1.datadef().domain()->valueType() == itINDEXEDITEM ) {
        rng = IndexedIdentifierRange::merge(def1.datadef().range<IndexedIdentifierRange>(), def1.datadef().range<IndexedIdentifierRange>());
    } else if (def1.datadef().domain()->valueType() == itNAMEDITEM ) {
        rng = NamedIdentifierRange::merge(def1.datadef().range<NamedIdentifierRange>(), def1.datadef().range<NamedIdentifierRange>(), renumberer);
    } else if (def1.datadef().domain()->valueType() == itTHEMATICITEM ) {
        rng = ThematicRange::merge(def1.datadef().range<ThematicRange>(), def1.datadef().range<ThematicRange>(), renumberer);
    }

    QString colname = def1.name() == FEATUREIDCOLUMN ? FEATUREIDCOLUMN : def1.name() + "_" + def2.name();
    ColumnDefinition coldef(colname, def1.datadef().domain());
    if ( rng)
        coldef.datadef().range(rng);

    return coldef;
}



