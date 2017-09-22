#include <QSqlQuery>
#include <QSqlError>
#include <QDir>

#include "kernel.h"
#include "ilwisangle.h"
#include "geos/geom/Coordinate.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "errorobject.h"
#include "ilwisdata.h"
#include "domainitem.h"
#include "domain.h"
#include "textdomain.h"
#include "itemdomain.h"
#include "numericrange.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "numericdomain.h"
#include "ilwis3connector.h"
#include "tableconnector.h"
#include "rawconverter.h"
#include "binaryilwis3table.h"
#include "domainconnector.h"
#include "ilwis3range.h"
#include "ilwiscontext.h"
#include "catalog.h"

using namespace Ilwis;
using namespace Ilwis3;

ConnectorInterface *TableConnector::create(const Resource &resource, bool load, const IOOptions &options) {
    return new TableConnector(resource, load, options);

}

TableConnector::TableConnector(const Resource &resource, bool load, const IOOptions &options) : Ilwis3Connector(resource, load, options)
{
   if ( options.contains("attributedomain"))
       attributeDomain(options["attributedomain"].toString());
}

bool TableConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    Locker<> lock(_mutex);
    _converters.clear();
    _selected.clear();

    if(!Ilwis3Connector::loadMetaData(data, options))
        return false;

    quint32 ncolumns = _odf->value("Table","Columns").toInt();
    quint32 rows = _odf->value("Table","Records").toInt();
    _attributeDomain = _odf->value("Table","Domain");
    QVector<ColumnDefinition>  columns;
    ColumnDefinition key  = getKeyColumn();
    if ( key.isValid()) {
        columns.push_back(key);
        if ( !options.contains("attributetable")) {
            _primaryKey = key.name();
        }else if (!options["attributetable"].toBool() == true){
             _primaryKey = key.name();
        }

    }

    Table *tbl = static_cast<Table *>(data);
    for(quint32 index=0; index < ncolumns; ++index) {
        QString colName = _odf->value("TableStore", QString("Col%1").arg(index));
        ColumnDefinition col = makeColumn(colName, index);
        tbl->addColumn(col);


    }
    if ( _primaryKey != sUNDEF){
        tbl->addColumn(key);
    }
    tbl->recordCount(rows);
    return true;

}

QString TableConnector::type() const
{
    return "Table";
}

IlwisObject *TableConnector::create() const
{
    return new FlatTable(_resource);
}

ColumnDefinition TableConnector::makeColumn(const QString& colName, quint64 index) {

    QString domName = _odf->value("Col:" + colName,"Domain");
    IDomain dom;
    bool attachPath = true;
    if ( isSystemObject(domName)) {
        domName = noExt(domName).toLower();
        domName = name2Code(domName, TypeHelper::type2name(itDOMAIN));
        attachPath = false;
    }
    if ( domName.toLower() == "string.dom"){
        domName = "code=domain:text";
        attachPath = false;
    }
    if ( attachPath){
        QString path =  _resource.container().toString();
        domName = path + "/" + domName;
    }
    if(!dom.prepare(domName,{"mustexist",true})) {
        return ColumnDefinition();
    }
    QString section = QString("Col:%1").arg(colName);
    RawConverter conv = Ilwis3Range::converter(_odf,section);
    _converters[colName] = conv;
    ColumnDefinition col(colName, dom, index );
    if ( dom->ilwisType() == itNUMERICDOMAIN){
        QString minmax = _odf->value(section,"MinMax");
        QString range = _odf->value(section,"Range");
        QStringList parts = range.split(":");
        double resolution = 1.0;
        if ( parts.size() > 3) {
            resolution = parts[2].toDouble();
        }
        if ( minmax != sUNDEF)
            parts = minmax.split(":");

        double vmin = parts[0].toDouble();
        double vmax = parts[1].toDouble();

        col.datadef().range(new NumericRange(vmin, vmax, resolution));


    }
    return col;
}

ColumnDefinition TableConnector::getKeyColumn() {
    QString domain = _odf->value("Table","Domain").toLower();
    if (domain == "none.dom" || domain == "none")
        return ColumnDefinition();
    if (domain.indexOf(".mpa") != -1 || domain.indexOf(".mps") != -1 || domain.indexOf(".mpp") !=-1){
        QFileInfo inf(QUrl(_odf->url()).toLocalFile());
        QString fn = inf.absolutePath() + "/" + domain;
        IniFile ini(fn);
        domain = ini.value("BaseMap", "Domain");
    }
    IThematicDomain dom;
    dom.prepare(filename2FullPath(domain, this->_resource));
    if ( !dom.isValid()) {
        kernel()->issues()->log(TR(ERR_NO_INITIALIZED_1).arg(domain));
        return ColumnDefinition();
    }
    QString name = dom->name();
    int index = name.indexOf(".");
    if ( index != -1)
        name = name.left(index);
    ColumnDefinition colKey(name, dom, 0);
    return colKey;

}

bool TableConnector::loadData(IlwisObject* data , const IOOptions &) {
    Locker<> lock(_mutex);

    Ilwis3::BinaryIlwis3Table tbl ;
    if (!tbl.load(_odf)) // no table found?
        return false;
    Table *table = static_cast<Table *>(data);

    if(!table->createTable())
        return false;
    table->dataLoaded(true); //  to prevent any succesfull calls of iniltload, we are loading here so no extra call needed

    int colindex = 0;
    while( colindex < tbl.columns()) {
        QString colName = tbl.columnName(colindex);
        ColumnDefinition col = table->columndefinition(colName);
        if ( col.isValid()) {
            std::vector<QVariant> varlist(tbl.rows());
            RawConverter conv = _converters[colName];
            IlwisTypes valueType = col.datadef().domain<>()->valueType();
            for(quint32 j = 0; j < tbl.rows(); ++j){
                if ( (valueType >= itINT8 && valueType <= itDOUBLE) || ((valueType & itDOMAINITEM) != 0)) {
                    double value;
                    if (tbl.get(j,colindex,value)) {
                        double v = conv.scale() == 0 ? value : conv.raw2real(value);
                        if ( v == iILW3UNDEF) // inconsistency of how column storetypes and undefs are defined
                            v = rUNDEF;
                        varlist[j] =  v;
                    }
                } else if (valueType == itSTRING ) {
                    QString value;
                    if (tbl.get(j,colindex,value)) {
                        varlist[j] = value;
                    }
                }
            }
            table->column(colName,varlist);
        }
        colindex++;
    }
    if ( _primaryKey != sUNDEF){
        QString domain = _odf->value("Table","Domain").toLower();
        if (domain != "none.dom" && domain != "none"){
            if (domain.indexOf(".mpa") != -1 || domain.indexOf(".mps") != -1 || domain.indexOf(".mpp") !=-1){
                QFileInfo inf(QUrl(_odf->url()).toLocalFile());
                QString fn = inf.absolutePath() + "/" + domain;
                IniFile ini(fn);
                domain = ini.value("BaseMap", "Domain");
            }
            IDomain dom;
            dom.prepare(filename2FullPath(domain, this->_resource));
            IItemDomain itemdom = dom.as<ItemDomain<DomainItem>>();
            std::vector<QVariant> varlist(tbl.rows());
            for(int i =0; i<itemdom->count(); ++i){
                Raw raw = itemdom->item(i)->raw();
                varlist[i] = raw;
            }
            table->column(_primaryKey,varlist);
        }
    }
    _binaryIsLoaded = true;

    return true;
}

bool TableConnector::storeBinaryData(IlwisObject *obj)
{
    const Table *tbl = static_cast<const Table *>(obj);


    int skip = iUNDEF;
    BinaryIlwis3Table ilw3tbl;
    std::ofstream output_file;
    QFileInfo inf(_resource.toLocalFile());
    if(!ilw3tbl.openOutput(inf.absolutePath() + "/" + inf.baseName()  + ".tb#", output_file))
        return false;

    for(int i=0; i < tbl->columnCount(); ++i) {
        const ColumnDefinition& def = const_cast<Table *>(tbl)->columndefinitionRef(i);
        ilw3tbl.addStoreDefinition(def.datadef());
    }
    quint32 reccount = _selected.size() > 0 ? _selected.size() :  tbl->recordCount();
    for(int y=0; y < reccount; ++y) {
        std::vector<QVariant> rec;
        if ( _selected.size() > 0 ){
            rec = tbl->record(_selected[y]);
        }
        else{
            rec = tbl->record(y);
        }

        ilw3tbl.storeRecord(output_file, rec, skip);

    }

    output_file.close();
    return true;
}

QString TableConnector::getDomainName(const IDomain& dom, bool& isSystem) {

    QString name = dom->code() != sUNDEF ? code2name(dom->code(), "domain") : sUNDEF;
    if ( name != sUNDEF)
        return name;
    isSystem = dom->isSystemObject();

    Resource res = dom->resource();
    QString url = res.url().toString();
    QString filename = url.mid(url.lastIndexOf("/") + 1);

    return filename;
}

bool TableConnector::storeMetaData(IlwisObject *obj, const IOOptions &options)
{
    const Table *tbl = static_cast<const Table *>(obj);

    if(!Ilwis3Connector::storeMetaData(obj, itTABLE))
        return false;

    int reduceColumns = 0; // the featured_id column will not be go the ilwis3, useless info at that level
    quint32 reccount = _selected.size() > 0 ? _selected.size() :  tbl->recordCount();
    QString domname = _attributeDomain;
    if ( domname == "" )
        domname = "none.dom";
    _odf->setKeyValue("Ilwis", "Type", "Table");
    _odf->setKeyValue("Ilwis", "Class", "Table");
    _odf->setKeyValue("Table", "Domain", domname);
    _odf->setKeyValue("Table", "DomainInfo", QString("%1;Long;UniqueID;0;;").arg(_attributeDomain));
    _odf->setKeyValue("Table", "Columns", IniFile::FormatElement(tbl->columnCount() - reduceColumns));
    _odf->setKeyValue("Table", "Records", IniFile::FormatElement(reccount));
    _odf->setKeyValue("Table", "Type", "TableStore");
    _odf->setKeyValue("TableStore", "Type", "TableBinary");
    _odf->setKeyValue("TableStore", "UseAs", "No");
    QFileInfo tblOdf(_resource.toLocalFile(true));
    QString dataFile = tblOdf.baseName() + ".tb#";
    _odf->setKeyValue("TableStore", "Data", dataFile);
    _odf->setKeyValue("TableStore", "StoreTime", Time::now().toString());
    if (storeColumns(tbl, options)){
        _odf->store("tbt", sourceRef().toLocalFile());
        return true;
    }
    return false;

}

bool TableConnector::storeColumns(const Table *tbl, const IOOptions &options) {
    for(int i=0; i < tbl->columnCount(); ++i) {
        ColumnDefinition def = tbl->columndefinition(i);
        IDomain dmColumn = def.datadef().domain<>();
        bool isOldSystem = true;
        QString domName = getDomainName(dmColumn, isOldSystem);
        if ( !isOldSystem) {
            if (domName.indexOf("/domains/") > -1)
                domName = def.name() + ".dom";
            if (domName.indexOf(ANONYMOUS_PREFIX) != -1) {
                // rename using columnname + domainID
                QString num = QString::number(dmColumn->id());
                domName =def.name() + "_" + num;
            }
            QString fileUrl = "file:///" + _odf->path()+ "/" + domName;
            if ( fileUrl.indexOf(".dom") == -1){
                fileUrl += ".dom";
            }
            if (QFileInfo(QUrl(fileUrl).toLocalFile()).exists()) {
                IDomain existingDomain(fileUrl);
                if (!dmColumn->isCompatibleWith(existingDomain.ptr(), true))
                    fileUrl = OSHelper::ensureUniqueFilename(QUrl(fileUrl).toLocalFile());
            }
            int lastSlash = fileUrl.lastIndexOf("/");
            domName = fileUrl.right(fileUrl.length() - lastSlash - 1);
            Resource res = dmColumn->resource();
            res.setUrl(fileUrl);
            res.setUrl(fileUrl,true);
            DomainConnector conn(res, false);
            conn.storeMetaData(dmColumn.ptr(), options);
        }
        QString colpostfix = def.name();
        if ( colpostfix.indexOf(QRegExp("[.]")) != -1)
            colpostfix = "'" + colpostfix + "'";
        _odf->setKeyValue("TableStore", QString("Col%1").arg(i), colpostfix);
        QString colName = QString("Col:%1").arg(colpostfix);
        _odf->setKeyValue(colName, "Time", Time::now().toString());
        _odf->setKeyValue(colName, "Version", "3.1");
        _odf->setKeyValue(colName, "Class", "Column");
        if (!( domName.indexOf(".mpa") != -1 || domName.indexOf(".mps")!= -1 || domName.indexOf(".mpp")!= -1 || domName.indexOf(".mpr")!= -1)){
            domName =  domName.indexOf(".dom") != -1 ? domName : domName + ".dom";
        }

        _odf->setKeyValue(colName, "Domain",domName);
        //_odf->setKeyValue(colName, "Time", Time::now().toString());
        _odf->setKeyValue(colName, "DomainChangeable", "Yes");
        _odf->setKeyValue(colName, "ValueRangeChangeable", "Yes");
        _odf->setKeyValue(colName, "ExpressionChangeable", "Yes");
        _odf->setKeyValue(colName, "ReadOnly", "No");
        _odf->setKeyValue(colName, "OwnedByTable", "Yes");
        QString domainInfo;
        if ( dmColumn->ilwisType() == itNUMERICDOMAIN) {
            domainInfo = storeNumericColumn(def, colName, domName);
        } else if ( dmColumn->valueType() == itTHEMATICITEM) {
            domainInfo = QString("%1;Int;class;256;;").arg(domName) ;
            _odf->setKeyValue(colName, "StoreType", "Long");
        } else if ( dmColumn->valueType() == itSTRING) {
            domainInfo = "string.dom;String;string;0;;";
            _odf->setKeyValue(colName, "StoreType","String");
        } else if ( dmColumn->valueType() & itIDENTIFIERITEM) {
            int count = dmColumn->range<ItemRange>()->count();
            domainInfo = QString("%1;Long;id;%2;;").arg(domName).arg(count) ;
            _odf->setKeyValue(colName, "StoreType", "Long");
        }
        _odf->setKeyValue(colName, "DomainInfo", domainInfo);
    }
    return true;
}

QString TableConnector::storeNumericColumn(const ColumnDefinition& def, const QString& colName, QString& domName) {
    IDomain dmColumn = def.datadef().domain();
    SPNumericRange numdmrange = dmColumn->range<NumericRange>();
    if ( numdmrange.isNull()){
        ERROR1(ERR_NO_INITIALIZED_1,TR("numeric range"));
        return sUNDEF;
    }
    SPNumericRange numrange = def.datadef().range<NumericRange>();
    if ( numrange.isNull()){
        numrange = def.datadef().domain()->range<NumericRange>();
    }
    RawConverter conv(numrange->min(), numrange->max(), numrange->resolution());
    double resolution = numrange->resolution();
    if ( domName == sUNDEF) {
        domName = "value.dom";
        if ( numdmrange->min() >= 0 && numdmrange->max() <=255 && resolution == 1)
            domName = "image.dom";
    }
    _odf->setKeyValue(colName, "Domain", domName.indexOf(".dom") != -1 ? domName : domName + ".dom");
    QString range;
    if ( resolution != 1)
        range = QString("%1:%2:%3:offset=%4").arg(numrange->min()).arg(numrange->max()).arg(resolution).arg(conv.offset());
    else
        range = QString("%1:%2:offset=%3").arg(numrange->min()).arg(numrange->max()).arg(conv.offset());

    _odf->setKeyValue(colName,"Range",range);
    QString storeType = "Real";
    if ( conv.storeType() & itINT32 )
        storeType = "Long";
   else if ( conv.storeType() & itINT16 )
        storeType = "Int"  ;
    else if ( conv.storeType() & itUINT8 )
         storeType = "Byte"  ;
    QString domainInfo = QString("%1;%2;value;0;%3;%4;0.1;offset=%5").arg(domName).
            arg(storeType).
            arg(IniFile::FormatElement(numdmrange->min())).
            arg(IniFile::FormatElement(numdmrange->max())).
            arg(conv.offset());
    _odf->setKeyValue(colName, "StoreType", storeType == "Real" ? "Real" : "Long");

    return domainInfo;
}

QString TableConnector::valueType2DataType(IlwisTypes ty) {
    QString vType=sUNDEF;
    if ( (ty >= itINT8 && ty <= itINT64) || ((ty & itDOMAINITEM) != 0)) {
        vType = "Integer"        ;
    } else if ( ty >= itINT8 && ty <= itDOUBLE) {
        vType = "Real";
    } else if ( ty == itSTRING) {
        vType = "Text";
    } else {
        //TODO: other domain types
    }
    return vType;
}
void TableConnector::selectedRecords(const std::vector<quint32> &recs) {
    _selected = recs;
}

void TableConnector::attributeDomain(const QString &attdom)
{
    _attributeDomain = attdom;
}

QString TableConnector::format() const
{
    return "table";
}
