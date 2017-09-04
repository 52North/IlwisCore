#include <QLibrary>
#include <QDebug>
#include <QFileInfo>
#include <QSettings>
#include <functional>
#include <QSqlQuery>
#include <QSqlError>
#include "kernel.h"
#include "errorobject.h"
#include "ilwiscontext.h"
#include "dataformat.h"
#include "supportlibraryloader.h"
#include "gdalproxy.h"
#include "oshelper.h"


Ilwis::Gdal::GDALProxy* Ilwis::Gdal::GDALProxy::_proxy = 0;

Ilwis::Gdal::GDALProxy* Ilwis::Gdal::gdal() {
    if (Ilwis::Gdal::GDALProxy::_proxy == 0) {
        Ilwis::Gdal::GDALProxy::_proxy = new Ilwis::Gdal::GDALProxy();
        Ilwis::Gdal::GDALProxy::_proxy->prepare();

    }
    if (!Ilwis::Gdal::GDALProxy::_proxy->isValid()) {
        throw InternalError(TR("Gdal library not properly initialized"));
    }
    return Ilwis::Gdal::GDALProxy::_proxy;
}

using namespace Ilwis;
using namespace Gdal;

GdalHandle::GdalHandle(void* h, GdalHandleType t, quint64 o): _handle(h),_type(t),_owner(o){}

GdalHandle::~GdalHandle()
{
}

GdalHandle::GdalHandleType GdalHandle::type(){
    return _type;
}
void* GdalHandle::handle(){
    return _handle;
}

GDALProxy::GDALProxy() {
    QFileInfo ilw = context()->ilwisFolder();
    QString path = ilw.canonicalFilePath() + "/extensions/gdalconnector" ;
    SupportLibraryLoader loader(path + "/resources/libraries.config");
    QString gdallibname = loader.order2name(100000);
    bool ok = false;
    if ( gdallibname != sUNDEF){
        if (QFileInfo(gdallibname).exists()) {
            _libgdal.setFileName(gdallibname);
        } else {
            // expect lib at ilwis folder
            _libgdal.setFileName(path + "/" + gdallibname);
        }
        ok = _libgdal.load();
    }
    if ( !ok){
        ERROR2(ERR_COULD_NOT_LOAD_2, TR("name"), "gdal connector");
    }
    _isValid = ok;
}

GDALProxy::~GDALProxy(){
//    foreach(GdalHandle* handle, _openedDatasets) {
//        if (handle->_type == GdalHandle::etGDALDatasetH){
//            close(handle->_handle);
//        }else{ //etOGRDataSourceH
//            if (OGRErr err = releaseDataSource(handle->_handle) != OGRERR_NONE){
//                ERROR2(ERR_INVALID_PROPERTY_FOR_2, QString("OGRDataSource (ERR %1)").arg(err), QString("%1").arg(handle->_owner));
//            }
//        }
//    }
}



bool GDALProxy::prepare() {
    //GDAL Raster
    close = add<IGDALClose>("GDALClose");
    registerAll = add<IGDALAllRegister>("GDALAllRegister");
    identifyDriver = add<IGDALIdentifyDriver>("GDALIdentifyDriver");
    open = add<IGDALOpen>("GDALOpen");
    xsize = add<IGDALGetSize>("GDALGetRasterXSize");
    ysize = add<IGDALGetSize>("GDALGetRasterYSize");
    layerCount = add<IGDALGetSize>("GDALGetRasterCount");
    getRasterBand = add<IGDALGetRasterBand>("GDALGetRasterBand");
    create = add<IGDALCreate>("GDALCreate");
    rasterDataType = add<IGDALGetRasterDataType>("GDALGetRasterDataType");
    getProjectionRef = add<IGDALGetProjectionRef>("GDALGetProjectionRef");
    setProjection = add<IGDALSetProjection>("GDALSetProjection");
    getGeotransform = add<IGDALGetGeoTransform>("GDALGetGeoTransform");
    setGeoTransform = add<IGDALSetGeoTransform>("GDALSetGeoTransform");
    rasterIO = add<IGDALRasterIO>("GDALRasterIO");
    getDataTypeSize = add<IGDALGetDataTypeSize>("GDALGetDataTypeSize");
    getAccess = add<IGDALGetAccess>("GDALGetAccess");
    getAttributeValue = add<IOSRGetAttrValue>("OSRGetAttrValue");
    getDriver = add<IGDALGetDriver>("GDALGetDriver");
    getGDALDriverByName = add<IGDALGetDriverByName>("GDALGetDriverByName");
    getDriverByDataSet = add<IGDALGetDatasetDriver>("GDALGetDatasetDriver");
    getDriverCount = add<IGDALGetDriverCount>("GDALGetDriverCount");
    getLongName = add<IGDALGetDriverName>("GDALGetDriverLongName");
    getShortName = add<IGDALGetDriverName>("GDALGetDriverShortName");
    getMetaDataItem = add<IGDALGetMetadataItem>("GDALGetMetadataItem");
    getMetaData = add<IGDALGetMetadata>("GDALGetMetadata");
    getGCPCount = add<IGDALGetGCPCountFunc>("GDALGetGCPCount");
    getGCPProjection = add<IGDALGetGCPProjectionFunc>("GDALGetGCPProjection");
    getGCPs = add<IGDALGetGCPsFunc>("GDALGetGCPs");
    minValue = add<IGDALRasValue>("GDALGetRasterMinimum");
    maxValue = add<IGDALRasValue>("GDALGetRasterMaximum");
    setUndefinedValue = add<IGDALSetRasterNoDataValue>("GDALSetRasterNoDataValue");
    getUndefinedValue = add<IGDALGetRasterNoDataValue>("GDALGetRasterNoDataValue");
    colorInterpretation = add<IGDALGetRasterColorInterpretation>("GDALGetRasterColorInterpretation");
    setColorInterpretation = add<IGDALSetRasterColorInterpretation>("GDALSetRasterColorInterpretation");
    getColorPaletteEntry = add<IGDALGetColorEntry>("GDALGetColorEntry");
    setColorPaletteEntry = add<IGDALSetColorEntry>("GDALSetColorEntry");
    setColorPalette = add<IGDALSetRasterColorTable>("GDALSetRasterColorTable");
    createColorPalette = add<IGDALCreateColorTable>("GDALCreateColorTable");
    getColorPalette = add<IGDALGetRasterColorTable>("GDALGetRasterColorTable");
    getColorPaletteSize = add<IGDALGetColorEntryCount>("GDALGetColorEntryCount");
    getPaletteColorInterpretation = add<IGDALGetPaletteInterpretation>("GDALGetPaletteInterpretation");
    //Open Spatial Reference
    importFromEpsg = add<IOSRImportFromEPSG>("OSRImportFromEPSG");
    exportToPrettyWkt = add<IOSRExportToPrettyWkt>("OSRExportToPrettyWkt");
    getProjectionParm = add<IOSRGetProjParm>("OSRGetProjParm");
    setWellKnownGeogCs = add<IOSRSetWellKnownGeogCS>("OSRSetWellKnownGeogCS");
    isProjected = add<IOSRIsProjectedFunc>("OSRIsProjected");
    getAuthorityCode = add<IOSRGetAuthorityCode>("OSRGetAuthorityCode");
    getAuthorityName = add<IOSRGetAuthorityName>("OSRGetAuthorityName");
    newSpatialRef = add<IOSRNewSpatialReference>("OSRNewSpatialReference");
    importFromWkt = add<IOSRImportFromWkt>("OSRImportFromWkt");
    exportToWkt = add<IExportToWkt>("OSRExportToWkt");
    importFromProj4 = add<IOSRImportFromProj4>("OSRImportFromProj4");
    export2Proj4 = add<IOSRExportToProj4>("OSRExportToProj4");
    releaseSRS = add<IOSRRelease>("OSRRelease");
    //OGR
    ogrOpen = add<IOGROpen>("OGROpen");
    releaseDataSource = add<IOGRReleaseDataSource>("OGRReleaseDataSource");
    ogrRegisterAll = add<IOGRRegisterAll>("OGRRegisterAll");
    ogrDriverCount =add<IOGRGetDriverCount>("OGRGetDriverCount");
    ogrGetDriver = add<IOGRGetDriver>("OGRGetDriver");
    getDriverByName = add<IOGRGetDriverByName>("OGRGetDriverByName");
    //OGR Driver
    getOGRDriverName = add<IOGRGetDriverName>("OGR_Dr_GetName");
    testDriverCapability = add<IOGRTestDriverCapability>("OGR_Dr_TestCapability");
    createDatasource = add<IOGR_Dr_CreateDataSource>("OGR_Dr_CreateDataSource");
    //OGR DataSource
    createOgrLayer = add<IOGR_DS_CreateLayer>("OGR_DS_CreateLayer");
    getLayerByName = add<IOGR_DS_GetLayerByName>("OGR_DS_GetLayerByName");
    getLayerCount = add<IGetLayerCount>("OGR_DS_GetLayerCount");
    getLayer = add<IGetLayer>("OGR_DS_GetLayer");
    destroyDataSource = add<IOGR_DS_Destroy>("OGR_DS_Destroy");
    //OGR Layer
    getLayerName = add<IGetLayerName>("OGR_L_GetName");
    getLayerGeometry = add<IGetLayerGeometryType>("OGR_L_GetGeomType");
    resetReading = add<IResetReading>("OGR_L_ResetReading");
    getNextFeature = add<IGetNextFeature>("OGR_L_GetNextFeature");
    getLayerDef = add<IGetLayerDefn>("OGR_L_GetLayerDefn");
    getSpatialRef = add<IGetSpatialRef>("OGR_L_GetSpatialRef");
    getFeatureCount = add<IGetFeatureCount>("OGR_L_GetFeatureCount");
    getLayerExtent = add<IGetLayerExtent>("OGR_L_GetExtent");
    getSpatialFilter = add<IOGRGetSpatialFilter>("OGR_L_GetSpatialFilter");
    addAttribute = add<IOGR_L_CreateField>("OGR_L_CreateField");
    getLayerSchema = add<IOGR_L_GetLayerDefn>("OGR_L_GetLayerDefn");
    addFeature2Layer = add<IOGR_L_CreateFeature>("OGR_L_CreateFeature");
    getRasterOffset = add<IGDALGetRasterOffset>("GDALGetRasterOffset");
    getRasterScale = add<IGDALGetRasterScale>("GDALGetRasterScale");


    //OGR FielDefinition
    getFieldCount = add<IGetFieldCount>("OGR_FD_GetFieldCount");
    getFieldDfn = add<IGetFieldDefn>("OGR_FD_GetFieldDefn");
    //OGR Field
    getFieldType = add<IGetFieldType>("OGR_Fld_GetType");
    getFieldName = add<IGetFieldName>("OGR_Fld_GetNameRef");
    createAttributeDefintion = add<IOGR_Fld_Create>("OGR_Fld_Create");
    destroyAttributeDefintion = add<IOGR_Fld_Destroy>("OGR_Fld_Destroy");
    //OGR Feature
    createFeature = add<IOGR_F_Create>("OGR_F_Create");
    getFieldIndex = add<IOGR_F_GetFieldIndex>("OGR_F_GetFieldIndex");
    setStringAttribute = add<IOGR_F_SetFieldString>("OGR_F_SetFieldString");
    setIntegerAttribute = add<IOGR_F_SetFieldInteger>("OGR_F_SetFieldInteger");
    setDoubleAttribute = add<IOGR_F_SetFieldDouble>("OGR_F_SetFieldDouble");
    setDateTimeAttribute = add<IOGR_F_SetFieldDateTime>("OGR_F_SetFieldDateTime");\
    getFieldAsString = add<IGetFieldAsString>("OGR_F_GetFieldAsString");
    getFieldAsDouble = add<IGetFieldAsDouble>("OGR_F_GetFieldAsDouble");
    getFieldAsInt = add<IGetFieldAsInteger>("OGR_F_GetFieldAsInteger");
    getFieldAsDateTime = add<IOGR_F_GetFieldAsDateTime>("OGR_F_GetFieldAsDateTime");\
    getGeometryRef = add<IGetGeometryRef>("OGR_F_GetGeometryRef");
    destroyFeature = add<IDestroyFeature>("OGR_F_Destroy");
    setGeometry = add<IOGR_F_SetGeometry>("OGR_F_SetGeometry");
    setGeometryDirectly = add<IOGR_F_SetGeometryDirectly>("OGR_F_SetGeometryDirectly");
    //OGR Geometry
    getGeometryType = add<IGetGeometryType>("OGR_G_GetGeometryType");
    getPointCount = add<IGetPointCount>("OGR_G_GetPointCount");
    getPoint = add<IGetPoints>("OGR_G_GetPoint");
    getSubGeometryCount = add<IGetSubGeometryCount>("OGR_G_GetGeometryCount");
    getSubGeometryRef = add<IGetSubGeometryRef>("OGR_G_GetGeometryRef");
    getEnvelope3D = add<IOGRGetEnvelope3D>("OGR_G_GetEnvelope3D");
    createGeometry = add<IOGR_G_CreateGeometry>("OGR_G_CreateGeometry");
    addPoint = add<IOGR_G_AddPoint>("OGR_G_AddPoint");
    addPoint2D = add<IOGR_G_AddPoint_2D>("OGR_G_AddPoint_2D");
    setPoint = add<IOGR_G_SetPoint>("OGR_G_SetPoint");
    setPoint2D = add<IOGR_G_SetPoint_2D>("OGR_G_SetPoint_2D");
    destroyGeometry = add<IOGR_G_DestroyGeometry>("OGR_G_DestroyGeometry");
    addGeometry = add<IOGR_G_AddGeometry>("OGR_G_AddGeometry");
    addGeometryDirectly = add<IOGR_G_AddGeometryDirectly>("OGR_G_AddGeometryDirectly");
    getCoordinateDimension = add<IOGR_G_GetCoordinateDimension>("OGR_G_GetCoordinateDimension");
    getDriverName = add<IOGR_Dr_GetName>("OGR_Dr_GetName");
    getDriverFromDS = add<IOGR_DS_GetDriver>("OGR_DS_GetDriver");

    //CPL
    pushFinderLocation = add<ICPLPushFinderLocation>("CPLPushFinderLocation");
    getLastErrorMsg = add<ICPLGetLastErrorMsg>("CPLGetLastErrorMsg");
    setCPLErrorHandler = add<ICPLSetErrorHandler>("CPLSetErrorHandler");
    //VSI
    vsiFileFromMem = add<IVSIFileFromMemBuffer>("VSIFileFromMemBuffer");
    vsiClose = add<IVSIFCloseL>("VSIFCloseL");
    free = add<IFree>("VSIFree");

    if ( _isValid) {
        setCPLErrorHandler(&cplErrorHandler);
        //raster extensions
        registerAll();
        int ndrivers = getDriverCount();
        for(int index = 0; index < ndrivers; ++index) {
            GDALDriverH driverH = getDriver(index);
            if ( driverH) {
                QString cext = getMetaDataItem(driverH,GDAL_DMD_EXTENSION,NULL);//raster extensions only
                if ( cext == "mpr/mpl") // ilwsi3 formats have their own handling
                    continue;
                QString create1 = getMetaDataItem(driverH,GDAL_DCAP_CREATE,NULL);//raster extensions only
                QString create2 = getMetaDataItem(driverH,GDAL_DCAP_CREATECOPY,NULL);//raster extensions only
                QString access = create1.size() == 0 && create2.size() == 0 ? "r" : "rc";
                DataFormat frm("gdal", getShortName(driverH), getLongName(driverH), cext, access, itRASTER);
                frm.store();
            }
        }

        QFileInfo ilw = context()->ilwisFolder();
        QString path = ilw.canonicalFilePath() + "/Extensions/gdalconnector/resources";
        pushFinderLocation(path.toLocal8Bit());
        //feature extensions
        ogrRegisterAll();

        DataFormat::setFormatInfo(path + "/ogr_formats.config","gdal");

        QVariantList extensions = DataFormat::getFormatProperties(DataFormat::fpEXTENSION, itFEATURE,"gdal");
        for(auto ext : extensions){
            _featureExtensions += "*." + ext.toString().toLower();

        }
        _featureExtensions.removeDuplicates();
        extensions = DataFormat::getFormatProperties(DataFormat::fpEXTENSION, itRASTER,"gdal");
        for(auto ext : extensions)
            _rasterExtensions += "*." + ext.toString().toLower();
        _rasterExtensions.removeDuplicates();
        _allExtensions += _rasterExtensions;
        _allExtensions += _featureExtensions;

    }

    return _isValid;
}

void GDALProxy::cplErrorHandler(CPLErr eErrClass, int err_no, const char *msg){
    switch(eErrClass){
        case CE_None: MESSAGE2("%1: %2",translateCPLE(err_no),QString(msg));break;
        case CE_Debug: DEBUG2("%1: %2",translateCPLE(err_no),QString(msg));break;
        case CE_Warning: WARN2("%1: %2",translateCPLE(err_no),QString(msg));break;
        case CE_Failure: ERROR2("CPL FAILURE(%1): %21",translateCPLE(err_no),QString(msg));break;
        case CE_Fatal: CRITICAL2("CPL FATAL(%1): %2",translateCPLE(err_no),QString(msg));
    }
}

void GDALProxy::cplDummyHandler(CPLErr , int , const char *){
}

QString GDALProxy::translateOGRERR(char ogrErrCode){
    switch (ogrErrCode){
        case OGRERR_NONE: return "OGRERR_NONE"; break;
        case OGRERR_NOT_ENOUGH_DATA: return "OGRERR_NOT_ENOUGH_DATA"; break;
        case OGRERR_NOT_ENOUGH_MEMORY: return "OGRERR_NOT_ENOUGH_MEMORY"; break;
        case OGRERR_UNSUPPORTED_GEOMETRY_TYPE: return "OGRERR_UNSUPPORTED_GEOMETRY_TYPE"; break;
        case OGRERR_UNSUPPORTED_OPERATION: return "OGRERR_UNSUPPORTED_OPERATION"; break;
        case OGRERR_CORRUPT_DATA: return "OGRERR_CORRUPT_DATA"; break;
        case OGRERR_FAILURE: return "OGRERR_FAILURE"; break;
        case OGRERR_UNSUPPORTED_SRS: return "OGRERR_UNSUPPORTED_SRS"; break;
        case OGRERR_INVALID_HANDLE: return "OGRERR_INVALID_HANDLE"; break;
        default: return "unknown OGR Error code!";
    }
}

QString GDALProxy::translateCPLE(int errCode){
    switch (errCode){
        case CPLE_None: return "CPLE_None"; break;
        case CPLE_AppDefined: return "CPLE_AppDefined"; break;
        case CPLE_OutOfMemory: return "CPLE_OutOfMemory"; break;
        case CPLE_FileIO: return "CPLE_FileIO"; break;
        case CPLE_OpenFailed: return "CPLE_OpenFailed"; break;
        case CPLE_IllegalArg: return "CPLE_IllegalArg"; break;
        case CPLE_NotSupported: return "CPLE_NotSupported"; break;
        case CPLE_AssertionFailed: return "CPLE_AssertionFailed"; break;
        case CPLE_NoWriteAccess: return "CPLE_NoWriteAccess"; break;
        case CPLE_UserInterrupt: return "CPLE_UserInterrupt"; break;
        case CPLE_ObjectNull: return "CPLE_ObjectNull"; break;
        default: return "unknown CPL Error code!";
    }
}

bool GDALProxy::isValid() const
{
    return _isValid;
}

QStringList GDALProxy::getExtensions(IlwisTypes tp) const
{
    if ( tp == itRASTER)
        return _rasterExtensions;
    if ( tp == itTABLE){
        return {"*.csv"};
    }
    else if ( tp == itFEATURE)
        return _featureExtensions;
    else if ( tp == (itFEATURE | itRASTER))
        return _allExtensions;
    return QStringList();
}


bool GDALProxy::supports(const Resource &resource) const{
    if ( hasType(resource.ilwisType(), itCATALOG))
        return true;

    auto testFunc = [&](const QFileInfo& inf) -> bool {
        QString ext = inf.suffix();
        QString filter = "*." + ext;
        if ( gdal()->getExtensions(itRASTER).contains(filter,Qt::CaseInsensitive))
            return true;
        if ( gdal()->getExtensions(itFEATURE).contains(filter,Qt::CaseInsensitive))
            return true;
        if ( gdal()->getExtensions(itTABLE).contains(filter,Qt::CaseInsensitive))
            return true;
        return false;
    };
    QString sAllIlwisExtensions = ".mpr.mpa.mps.mpp.tbt.mpl.ioc.mpv.ilo.atx.grh.dom.rpr.grf.csy.his.hsa.hss.hsp.sms.stp.smc.ta2.mat.fil.fun.isl";
    if (! testFunc(resource.toLocalFile()))   {
        QFileInfo info(resource.container(true).toLocalFile()); // possible case that the container is a gdal catalog
        if (info.isFile() && testFunc(info)) // for the moment a gdal catalog has to be another file
            return true;
        else {
            QFileInfo info (resource.toLocalFile());
            QString ext = info.suffix();
            if (ext != "" && sAllIlwisExtensions.contains("." + ext))
                return false;
            else
                return 0 != gdal()->identifyDriver(resource.toLocalFile().toLocal8Bit(), 0); // last resort, let GDAL actually probe the file
        }
    }

    return true;

}

GdalHandle* GDALProxy::openFile(const QFileInfo& filename, quint64 asker, GDALAccess mode, bool message){
    void* handle = nullptr;
    auto name = filename.absoluteFilePath();
    if (message){
        setCPLErrorHandler(GDALProxy::cplErrorHandler);
    }else
        setCPLErrorHandler(GDALProxy::cplDummyHandler);

    if (_openedDatasets.contains(name)){
        return _openedDatasets[name];
    } else {
        handle = gdal()->ogrOpen(name.toLocal8Bit(), mode, NULL);
        if (handle){
            return _openedDatasets[name] = new GdalHandle(handle, GdalHandle::etOGRDataSourceH, asker);
        }else{
            handle = gdal()->open(name.toLocal8Bit(), mode);
            if (handle){
                return _openedDatasets[name] = new GdalHandle(handle, GdalHandle::etGDALDatasetH, asker);
            }else{
                if ( message)
                    ERROR1(ERR_COULD_NOT_OPEN_READING_1,name);
                return NULL;
            }
        }
    }
}

GdalHandle* GDALProxy::openUrl(const QUrl& url, quint64 asker, GDALAccess mode, bool message){
    void* handle = nullptr;
    auto name = QUrl::fromPercentEncoding(url.toString(QUrl::None).toLocal8Bit());
    if (message){
        setCPLErrorHandler(GDALProxy::cplErrorHandler);
    }else
        setCPLErrorHandler(GDALProxy::cplDummyHandler);
    bool ok;
    name.toInt(&ok);
    if (_openedDatasets.contains(name)){
        return _openedDatasets[name];
    } else if ( ok){
        Resource res = mastercatalog()->id2Resource(asker);
        QString localFile = QFileInfo(res.container(true).toLocalFile()).absoluteFilePath();
        handle = gdal()->open(localFile.toLocal8Bit(), mode);
        if (handle){
            return _openedDatasets[localFile] = new GdalHandle(handle, GdalHandle::etGDALDatasetH, asker);
        }
    }
    else {
        handle = gdal()->ogrOpen(name.toLocal8Bit(), mode, NULL);
        if (handle){
            return _openedDatasets[name] = new GdalHandle(handle, GdalHandle::etOGRDataSourceH, asker);
        }else{
            handle = gdal()->open(name.toLocal8Bit(), mode);
            if (handle){
                return _openedDatasets[name] = new GdalHandle(handle, GdalHandle::etGDALDatasetH, asker);
            }else{
                if ( message)
                    ERROR1(ERR_COULD_NOT_OPEN_READING_1,name);
                return NULL;
            }
        }

    }
    return 0;
}

void GDALProxy::closeFile(const QString &filename, quint64 asker){
    QString name = filename;
    auto iter = _openedDatasets.find(name);
    if (iter != _openedDatasets.end() && iter.value()->_owner == asker) {
        GdalHandle* handle = _openedDatasets[name];
        if (handle->type() == GdalHandle::etGDALDatasetH){
            close(handle->handle());
        }else if(handle->etOGRDataSourceH){
            if (/*OGRErr err = */releaseDataSource((OGRDataSourceH)handle->handle()) != OGRERR_NONE){
                //TODO everything seems to work but, the error remains!
                //ERROR2("Couldn't release OGRDataSource (OGRERR %1) for %2", translateOGRERR(err) , name);
            }
        }else{
            ERROR2(ERR_INVALID_PROPERTY_FOR_2, "GDAL-OGR HandleType", name);
        }
        _openedDatasets.remove(name);
    }
}

OGRSpatialReferenceH GDALProxy::srsHandle(GdalHandle* handle, const QString& source, bool message) {
    if (handle != nullptr){
        OGRSpatialReferenceH srshandle = nullptr;
        if (handle->_type == GdalHandle::etGDALDatasetH){
            srshandle = newSpatialRef(NULL);
            char *wkt = const_cast<char*>(getProjectionRef(handle->handle()));
            if (wkt) {
                if ( importFromWkt(srshandle, &wkt) == OGRERR_NONE ){
                    return srshandle;
                }else{
                    if ( message)
                        kernel()->issues()->log(TR(ERR_NO_OBJECT_TYPE_FOR_2).arg("CoordinateSystem", source), IssueObject::itWarning);
                    return NULL;
                }
            }else{
                if ( message)
                    kernel()->issues()->log(TR("Invalid or empty WKT for %1 %2").arg("CoordinateSystem", source), IssueObject::itWarning);
                return NULL;
            }
        }else if(handle->_type == GdalHandle::etOGRDataSourceH){
            OGRLayerH hLayer = getLayer(handle->handle(), 0);//take the first layer
            if (hLayer){
                srshandle = getSpatialRef(hLayer);
                if ( srshandle ){
                    return srshandle;
                }else{
                    if (message)
                        kernel()->issues()->log(TR(ERR_NO_OBJECT_TYPE_FOR_2).arg("CoordinateSystem", source), IssueObject::itWarning);
                    return NULL;
                }
            }else{
                if ( message)
                    kernel()->issues()->log(TR(ERR_NO_OBJECT_TYPE_FOR_2).arg("CoordinateSystem", source), IssueObject::itWarning);
                return NULL;
            }
        }else{
            if ( message)
                kernel()->issues()->log(TR(ERR_NO_OBJECT_TYPE_FOR_2).arg("CoordinateSystem", QString("%1 : nullptr").arg(source)), IssueObject::itWarning);
            return NULL;
        }
    }else{
        return NULL;
    }
}

void GDALProxy::releaseSrsHandle(GdalHandle *handle, OGRSpatialReferenceH srshandle, const QString &source){
    if (handle->_type == GdalHandle::etGDALDatasetH){
        releaseSRS(srshandle);
    }else if(handle->_type == GdalHandle::etOGRDataSourceH){
        //nothing by now
    }
}

Envelope GDALProxy::envelope(GdalHandle* handle, int index, bool force){

    Envelope bbox;
    if (handle->type() == GdalHandle::etGDALDatasetH){
        double geosys[6];
        CPLErr err = gdal()->getGeotransform(handle->handle(), geosys) ;
        if ( err != CE_None) {
            return bbox;
        }

        double a1 = geosys[0];
        double b1 = geosys[3];
        double a2 = geosys[1];
        double b2 = geosys[5];
        Pixel pix(gdal()->xsize(handle->handle()), gdal()->ysize(handle->handle()));
        Coordinate crdLeftup( a1 , b1);
        Coordinate crdRightDown(a1 + pix.x * a2, b1 + pix.y * b2 ) ;
        Coordinate cMin( min(crdLeftup.x, crdRightDown.x), min(crdLeftup.y, crdRightDown.y));
        Coordinate cMax( max(crdLeftup.x, crdRightDown.x), max(crdLeftup.y, crdRightDown.y));
        bbox = Envelope(cMin, cMax) ;
    }else {
        OGRLayerH layerH = gdal()->getLayer(handle->handle(),index);
        if ( layerH){


            OGREnvelope envelope;//might sometimes be supported as 3D now only posssible from OGRGeometry
            OGRErr err = gdal()->getLayerExtent(layerH, &envelope , force);//TRUE to FORCE
            if (err == OGRERR_NONE){
                bbox = Envelope(Coordinate(envelope.MinX,envelope.MinY),Coordinate(envelope.MaxX,envelope.MaxY));
            }
        }
    }
    return bbox;
}

IlwisTypes GDALProxy::translateOGRType(OGRwkbGeometryType type){
    IlwisTypes ret = itUNKNOWN;

    if ( type == wkbPoint || type == wkbMultiPoint || type == wkbPoint25D || type == wkbMultiPoint25D)
        ret += itPOINT;

    if ( type == wkbLineString || type == wkbMultiLineString || type == wkbLineString25D || type == wkbMultiLineString25D)
        ret += itLINE;

    if ( type == wkbPolygon || type == wkbMultiPolygon || type == wkbPolygon25D || type == wkbMultiPolygon25D)
        ret += itPOLYGON;

    if ( type == wkbGeometryCollection )
        ret = itFEATURE;

    return ret;
}
