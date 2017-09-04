#include <QSqlQuery>
#include <QSqlError>
#include <QRegExp>
#include <QColor>

#include "kernel.h"
#include "raster.h"
#include "module.h"
#include "featurecoverage.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "ilwisdata.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "columndefinition.h"
#include "table.h"
#include "rawconverter.h"
#include "ilwis3connector.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "tableconnector.h"
#include "colorrange.h"
#include "coordinatesystemconnector.h"
#include "coverageconnector.h"
#include "odfitem.h"

using namespace Ilwis;
using namespace Ilwis3;

CoverageConnector::CoverageConnector(const Resource &resource, bool load, const IOOptions &options) : Ilwis3Connector(resource, load, options)
{
}

bool CoverageConnector::getRawInfo(const QString& range, double& vmin, double& vmax, double& scale, double& offset) const{
    QStringList parts = range.split(":");
    if ( parts.size() >= 2) {
        bool ok1, ok2;
        vmin = parts[0].toDouble(&ok1);
        vmax = parts[1].toDouble(&ok2);
        if ( (ok1 && ok2) == false) {
            return ERROR1(ERR_ILLEGAL_VALUE_RANGE_DEFINITION_1,_resource.name());
        }
        scale = 1.0;
        offset = 0;
        if ( parts.size() == 3) {
            offset = parts[2].mid(7).toDouble();
        } if ( parts.size() == 4) {
            scale = parts[2].toDouble();
            offset = parts[3].mid(7).toDouble();
        }
         const_cast<CoverageConnector *>(this)->_converter = RawConverter(offset, scale, vmin, vmax);
         return true;
    }
    return false;
}

ITable CoverageConnector::prepareAttributeTable(const QString& file, const QString& basemaptype, const IOOptions &options) const{

    ITable attTable;
    if ( file != sUNDEF) {
        QString newfile = filename2FullPath(file, this->_resource);
        if(!attTable.prepare(OSHelper::neutralizeFileName(newfile), {"attributetable",true})){
            kernel()->issues()->log(file,TR(ERR_NO_INITIALIZED_1).arg(file),IssueObject::itWarning);
            return ITable();
        }
    }

    if ( basemaptype != "Map" ) {
        QString name = _resource.name();
        int index = -1;
        if ( (index = name.indexOf(".")) != -1){
            name = name.left(index);
        }
    }
    if ( !attTable.isValid())
        attTable.prepare();

    bool isNumeric = _odf->value("BaseMap","Range") != sUNDEF;
    if ( isNumeric){
        IDomain featuredom("value");
        attTable->addColumn(FEATUREVALUECOLUMN,featuredom);
    }
    else if ( attTable->columnIndex(COVERAGEKEYCOLUMN) == iUNDEF) { // external tables might already have these
        DataDefinition def = determineDataDefintion(_odf, options);
        if ( !def.isValid())
            return ITable();
        attTable->addColumn(ColumnDefinition(COVERAGEKEYCOLUMN,def, attTable->columnCount()));
    }
    return attTable;

}

bool CoverageConnector::loadMetaData(Ilwis::IlwisObject *data,const IOOptions& options)
{
    Ilwis3Connector::loadMetaData(data, options);

    Coverage *coverage = static_cast<Coverage *>(data);
    QString csyName = _odf->value("BaseMap","CoordSystem");
    ICoordinateSystem csy;
    if ( data->resource().hasProperty("coordinatesystem")){
        quint64 id = data->resource()["coordinatesystem"].toULongLong();
        Resource res = mastercatalog()->id2Resource(id);
        if ( res.isValid())
            csy.prepare(res);
    }

    if (!csy.isValid()){
        if ( csyName == sUNDEF){
            //due to inconsistent spelling we have to check other cases
            csyName = _odf->value("BaseMap","Coordsystem");
        }
        if ( csyName.toLower() == "latlonwgs84.csy")
            csyName = "code=epsg:4326";
        else if ( csyName.toLower() == "unknown.csy")
            csyName = "code=csy:unknown";
        else{
            csyName = filename2FullPath(csyName, this->_resource);
            addToMasterCatalog(csyName, itCOORDSYSTEM);
        }

        if ( !csy.prepare(csyName, itCOORDSYSTEM, options)) {
            kernel()->issues()->log(csyName,TR("Coordinate system couldnt be initialized, defaulting to 'unknown'"),IssueObject::itWarning);
            QString resource = QString("code=csy:unknown");
            if (!csy.prepare(resource)) {
                kernel()->issues()->log(TR("Fallback to 'unknown' failed, corrupt system files defintion"));
                return false;
            }
        }
    }
    coverage->coordinateSystem(csy);


    QString attfile = _odf->value("BaseMap", "AttributeTable");
    QString basemaptype = _odf->value("BaseMap", "Type");


    if ( attfile == sUNDEF){ // the attribute table might be in the domain
            QString domname = _odf->value("BaseMap", "Domain");
            domname = _odf->fileInfo().absolutePath() + "/" + domname;
            QFileInfo inf(domname);
            if ( inf.exists()){
                IniFile ini(inf.absoluteFilePath());
                attfile = ini.value("DomainSort","AttributeTable");
            }
    }
    bool isSortWithoutAtTab =  basemaptype == "Map" &&
            hasType(static_cast<RasterCoverage *>(coverage)->datadef().domain()->ilwisType(),itITEMDOMAIN) &&
            attfile == sUNDEF;
    if ( basemaptype != "Map" || attfile != sUNDEF || isSortWithoutAtTab ) {
        ITable tbl = prepareAttributeTable(attfile, basemaptype, options);
        if ( tbl.isValid()){
            coverage->setAttributes(tbl);
            if ( attfile != sUNDEF)
                addProperty("TableRecordCount", tbl->recordCount()); // pass to caller; this is the correct featureCount for ilwis3 vector files
            if ( basemaptype == "Map"){
                  if ( isSortWithoutAtTab){
                    auto *raster = static_cast<RasterCoverage *>(coverage);
                    IItemDomain domain = raster->datadef().domain().as<ItemDomain<DomainItem>>();
                    std::vector<QVariant> values;
                    for(auto item : domain){
                        values.push_back(item->raw());
                    }
                    tbl->column(0,values);
                }
            }

        } else
            return false;
    }

    QString cbounds = _odf->value("BaseMap","CoordBounds");
    if ( cbounds == sUNDEF) // possible inconsistent spelling
        cbounds = _odf->value("BaseMap","Coordbounds");

    QStringList parts = cbounds.split(" ", QString::SkipEmptyParts);
    if ( parts.size() == 4) {
        double minx = parts[0].toDouble();
        double miny = parts[1].toDouble();
        double maxx = parts[2].toDouble();
        double maxy = parts[3].toDouble();
        Envelope env(Coordinate(minx, miny), Coordinate(maxx, maxy));
        coverage->envelope(env);
    } else {
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("Coordinate boundaries", data->name()), IssueObject::itWarning);
    }


    return true;
}

bool CoverageConnector::storeMetaData(IlwisObject *obj, IlwisTypes type, const Ilwis::DataDefinition &datadef, const QString& baseName)
{
    Ilwis::IDomain dom = datadef.domain<>();
    if (!dom.isValid())
        return ERROR2(ERR_NO_INITIALIZED_2, "Domain", obj->name());

    bool ok = Ilwis3Connector::storeMetaData(obj, type);
    if ( !ok)
        return false;

    Coverage *coverage = static_cast<Coverage *>(obj);

    const ICoordinateSystem csy = coverage->coordinateSystem();
    if (!csy.isValid())
        return ERROR2(ERR_NO_INITIALIZED_2, "CoordinateSystem", coverage->name());

    // write the corresponding coordinate system
    _csyName = writeCsy(obj, csy);
    _odf->setKeyValue("BaseMap", "CoordSystem", _csyName);

    Envelope bounds = coverage->envelope();
    if(!bounds.isValid()){
        ERROR2(ERR_NO_INITIALIZED_2, "Bounds", csy->name());
        return sUNDEF;
    }

    _odf->setKeyValue("BaseMap", "CoordBounds", QString("%1 %2 %3 %4").
                      arg(bounds.min_corner().x,10,'f').
                      arg(bounds.min_corner().y,10,'f').
                      arg(bounds.max_corner().x,10,'f').
                      arg(bounds.max_corner().y,10,'f'));

    calcStatistics(obj,NumericStatistics::pBASIC);

    if ( dom->ilwisType() == itNUMERICDOMAIN) {

        quint16 digits = coverage->statistics().significantDigits();
        qint32 delta = coverage->statistics()[NumericStatistics::pDELTA];
        _domainName = dom->name();

        double resolution = datadef.range()->as<NumericRange>()->resolution();

        if ((resolution != 0) &&  delta >= 0 && delta < 256 && digits == 0){
            if ( dom->code() == "boolean"){
                _domainName = "bool";
                _domainInfo = QString("bool.dom;Byte;bool;0;;");
                _odf->setKeyValue("BaseMap","DomainInfo",_domainInfo);
                _odf->setKeyValue("BaseMap","Range","0:1:offset=-1");
                _odf->setKeyValue("BaseMap","Domain","bool.dom");
            }
            else{
                _domainName = "image.dom";
                _domainInfo = QString("Image.dom;Byte;image;0;;");
                _odf->setKeyValue("BaseMap","DomainInfo",_domainInfo);
                _odf->setKeyValue("BaseMap","Range","0:255:offset=0");
                _odf->setKeyValue("BaseMap","MinMax","0:255");
                _odf->setKeyValue("BaseMap","Domain","Image.dom");
            }
        }
        else {
            const NumericStatistics& stats = coverage->statistics();
            double precision = (resolution == 0.0) ? resolution : pow(10, -stats.significantDigits());
            if (precision < 1e-06)
                precision = 0.0;
            RawConverter conv(stats[NumericStatistics::pMIN], stats[NumericStatistics::pMAX], precision);           
            QString storeType;
            if ( delta >= 0 && delta < 256 &&  resolution == 1){
                storeType = "Byte";
            } else if ( conv.storeType() == itUINT8){
                storeType = "Byte";
            } else if ( conv.storeType() == itINT16){
                storeType = "Int";
            } else if ( conv.storeType() == itINT32){
                storeType = "Long";
            } else if ( conv.storeType() == itDOUBLE){
                storeType = "Real";
            }
            _odf->setKeyValue("MapStore","Type",storeType);
            _odf->setKeyValue("BaseMap","Domain",_domainName);
            _odf->setKeyValue("BaseMap","MinMax",QString("%1:%2").arg(stats[NumericStatistics::pMIN]).arg(stats[NumericStatistics::pMAX]));
            _domainInfo = QString("%1;%2;value;0;%3;%4;%5:offset=%6").arg(_domainName).arg(storeType).arg(dom->range<NumericRange>()->min()).arg(dom->range<NumericRange>()->max()).arg(dom->range<NumericRange>()->resolution()).arg(conv.offset());
            _odf->setKeyValue("BaseMap","DomainInfo",_domainInfo);
            QString rng = QString("%1:%2:%3:offset=%4").arg(stats[NumericStatistics::pMIN] - precision).arg(stats[NumericStatistics::pMAX] + precision).arg(precision).arg(conv.offset());
            _odf->setKeyValue("BaseMap","Range",rng);
        }
    }
    if ( dom->ilwisType() == itITEMDOMAIN) {
         if ( hasType(dom->valueType(),itTHEMATICITEM | itNUMERICITEM) && coverage->ilwisType() == itRASTER) {
            _domainName =  Resource::toLocalFile(dom->resource().url(true), true);
            if ( _domainName == sUNDEF){
                if ( baseName != sUNDEF)
                    _domainName = QFileInfo(baseName).baseName() + ".dom";
                else{
                    _domainName = dom->name();
                    if ( _domainName.indexOf(".dom") == -1)
                        _domainName += ".dom";
                }

            }
            IThematicDomain themdom = dom.as<ThematicDomain>();
            if ( themdom.isValid()) {
                _domainInfo = QString("%1;Byte;class;%2;;").arg(_domainName).arg(themdom->count());
                _odf->setKeyValue("BaseMap","DomainInfo",_domainInfo);
                _odf->setKeyValue("BaseMap","Domain",_domainName);
            }
            _itemCount = themdom->count();
         } else if(dom->valueType() == itINDEXEDITEM) {
             if ( hasType(type, itFEATURE)){
                 _domainName = QFileInfo(QUrl(_odf->url()).toLocalFile()).fileName();
                 _domainInfo = QString("%1;Long;UniqueID;0;;").arg(_domainName);
                 _odf->setKeyValue("BaseMap","DomainInfo",_domainInfo);
                 _odf->setKeyValue("BaseMap","Domain",_domainName);
             }else {
                QFileInfo inf(QUrl(_odf->url()).toLocalFile());
                QString domName = inf.baseName() + ".dom";
                QUrl domPath = QUrl::fromLocalFile(inf.absoluteDir().absolutePath() + "/" + domName);
                dom->connectTo(domPath, "domain", "ilwis3", IlwisObject::cmOUTPUT);
                dom->store();
                _domainInfo = QString("%1;Long;UniqueID;0;;").arg(domName);
                _odf->setKeyValue("BaseMap","DomainInfo",domName);
                _odf->setKeyValue("BaseMap","Domain",domName);
             }
         } else if ( dom->valueType() == itNAMEDITEM) {
             INamedIdDomain iddom = dom.as<NamedIdDomain>();
             if ( baseName != sUNDEF)
                 _domainName = baseName + ".dom"; // Resource::toLocalFile(dom->source().url(), true);
             else{
                 _domainName = !iddom->isAnonymous() ? dom->name() : coverage->name();
                 if (_domainName.indexOf(".dom") == -1)
                     _domainName += ".dom";
             }
            _domainName = QFileInfo(_domainName).fileName();
            _domainInfo = QString("%1;Int;id;%2;;").arg(_domainName).arg(iddom->count());
            _odf->setKeyValue("BaseMap","DomainInfo",_domainInfo);
            _odf->setKeyValue("BaseMap","Domain",_domainName);
            QUrl url = makeUrl(_odf->url(),_domainName);
            iddom->connectTo(url,"domain","ilwis3", IlwisObject::cmOUTPUT);
            iddom->store({"storemode",Ilwis::IlwisObject::smMETADATA | Ilwis::IlwisObject::smBINARYDATA});
            _itemCount = iddom->count();
        }
    }
    QFileInfo inf(_domainName);
    _domainName = inf.fileName();

    return true;
}



TableConnector *CoverageConnector::createTableStoreConnector(ITable& attTable, Coverage *coverage, IlwisTypes tp, const QString& baseName) {
    QString dataFile = baseName;
    QString attDom = dataFile;
    if ( hasType(tp,itRASTER)) {
        RasterCoverage *raster = static_cast<RasterCoverage *>(coverage);
        Resource resource = raster->datadef().domain<>()->resource();
        if ( hasType(raster->datadef().domain<>()->ilwisType(),itITEMDOMAIN)){
            attDom = baseName + ".dom";
        }else {
            QFileInfo inf(resource.toLocalFile());
            attDom = inf.fileName();
        }
    }
    int index = dataFile.lastIndexOf(".");
    if ( index != -1) {
        dataFile = dataFile.left(index);
    }else{
        int idx = attDom.lastIndexOf(".");
        if ( idx == -1){
            if ( tp == itPOLYGON)
                attDom += ".mpa";
            else if ( tp == itRASTER)
                attDom += ".mpr";
            else if ( tp == itPOINT)
                attDom += ".mpp";
            else if ( tp == itLINE)
                attDom += ".mps";
            else
                attDom  += ".dom";
        }
    }
    if ( attTable->columnCount() > 0) { // one column means only featurid which we dont save.
        QString dataFilePath = context()->workingCatalog()->filesystemLocation().toString() + "/" + dataFile + ".tbt";
        QFileInfo inf(dataFile);
        _odf->setKeyValue("BaseMap","AttributeTable",dataFile + ".tbt");
//        attTable->setName(inf.baseName()); // why should we chance the name of the table to a local filename here ?

        IOOptions opt({"attributedomain"}, _domainName == "" ?  attDom : _domainName);
        attTable->connectTo(QUrl(dataFilePath),"table","ilwis3", IlwisObject::cmOUTPUT,opt);
        TableConnector *conn = tp == itRASTER ? 0 : new TableConnector(Resource(dataFilePath, itTABLE), false, opt);
        //TableConnector *conn = new TableConnector(Resource(dataFilePath, itTABLE), false);
        return conn;
    }
    return 0;
}

DataDefinition CoverageConnector::determineDataDefintion(const ODF& odf,  const IOOptions &options) const{
    IDomain dom;
    QString filename;
    QString domname = odf->value("BaseMap","Domain");
    if (domname != sUNDEF) { // probe if domain is an external file
        if ( domname == "Color.dom"){ // special case
                QColor clrMin = QColor(0,0,0);
                QColor clrMax = QColor(255,255,255);
                Range *colorRange = new ContinuousColorRange( clrMin, clrMax, ColorRangeBase::cmRGBA);
                return DataDefinition(IDomain("color"), colorRange);
        }else {
            filename = filename2FullPath(domname, this->_resource); // use an eventual absolute-path supplied in domname, otherwise look for it in the same folder as the BaseMap
            if (!QFileInfo(QUrl(filename).toLocalFile()).exists()) {
                filename = context()->workingCatalog()->resolve(domname, itDOMAIN); // if it is also not there, look for it in the working catalog (it might be different than the location of the BaseMap)
                if (!QFileInfo(QUrl(filename).toLocalFile()).exists())
                    filename = name2Code(domname, "domain"); // probe if it is a system file; note that this is a code, not a filename
            } else { // handle the case whereby we open a loose coverage file that is not in the mastercatalog; the domain must be added to the mastercatalog at this point, otherwise it will not load correctly by the "prepare" that follows, due to an incomplete "Resource"
                addToMasterCatalog(filename, itDOMAIN);
            }
        }
    } else
        filename = odf->url(); // probe if it is an internal domain
    if (!dom.prepare(filename, options)) {
        ERROR2(ERR_NO_INITIALIZED_2,"domain",odf->url());
        return DataDefinition();
    }
    DataDefinition def(dom);
    double vmax,vmin,scale,offset;
    QString range = odf->value("BaseMap","Range");
    if ( range != sUNDEF ) {
        if ( getRawInfo(range, vmin,vmax,scale,offset)) {
            QString range = odf->value("BaseMap","MinMax"); // this might be there, not necessarily. its info is better
            if ( range != sUNDEF){
                QStringList parts = range.split(":");
                if ( parts.size() == 2){
                    vmin = parts[0].toDouble();
                    vmax = parts[1].toDouble();
                }
            }
            def.range(new NumericRange(vmin, vmax,scale));
        }
    }
    return def;
}

void CoverageConnector::addToMasterCatalog(QString filename, IlwisTypes it) const {
    auto resource = mastercatalog()->name2Resource(filename, it);
    if (!resource.isValid()) {
        std::vector<Resource> itemsToAdd;
        ODFItem odfItem(QFileInfo(QUrl(filename).toLocalFile()));
        itemsToAdd.push_back(odfItem);
        mastercatalog()->addItems(itemsToAdd);
    }
}

