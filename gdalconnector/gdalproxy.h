#ifndef GDALPROXY_H
#define GDALPROXY_H

#include <QHash>
#include <QLibrary>

#include "gdal/ogr_api.h"
#include "gdal/gdal.h"
#include "gdal/cpl_vsi.h"

#include "geometries.h"

namespace Ilwis{

namespace Gdal{

typedef void *(*IGDALClose)(GDALDatasetH) ;
typedef void *(*IGDALAllRegister )() ;
typedef GDALDriverH (*IGDALIdentifyDriver )(const char *, char **) ;
typedef GDALDatasetH (*IGDALOpen )(const char *, GDALAccess) ;
typedef int (*IGDALGetSize )(GDALDatasetH) ;
typedef GDALRasterBandH (*IGDALGetRasterBand )(GDALDatasetH, int) ;
typedef GDALDatasetH (*IGDALCreate )(GDALDriverH hDriver, const char *, int, int, int, GDALDataType, char **) ;
typedef GDALDataType (*IGDALGetRasterDataType )(GDALRasterBandH) ;
typedef char * (*IGDALGetProjectionRef )(GDALDatasetH) ;
typedef OGRSpatialReferenceH (*IOSRNewSpatialReference )(const char *) ;
typedef CPLErr (*IGDALSetProjection)(GDALDatasetH,const char *);
typedef CPLErr (*IGDALGetGeoTransform )(GDALDatasetH, double *) ;
typedef CPLErr (*IGDALSetGeoTransform)(GDALDatasetH, double * );
typedef CPLErr (*IGDALRasterIO )(GDALRasterBandH , GDALRWFlag , int , int , int , int , void *, int , int , GDALDataType , int , int ) ;
typedef int (*IGDALGetDataTypeSize )(GDALDataType) ;
typedef int (*IGDALGetAccess )(GDALDatasetH) ;
typedef GDALDriverH (*IGDALGetDriver )(int) ;
typedef int (*IGDALGetDriverCount )() ;
typedef GDALDriverH (*IGDALGetDriverByName)(const char * ) ;
typedef GDALDriverH (*IGDALGetDatasetDriver)(GDALDatasetH );
typedef const char* (*IGDALGetDriverName )(GDALDriverH) ;
typedef const char* (*IGDALGetMetadataItem )(GDALMajorObjectH , const char *, const char *) ;
typedef char ** (*IGDALGetMetadata)(GDALMajorObjectH , const char *);
typedef double (*IGDALRasValue)(GDALRasterBandH, int * );
typedef GDALColorInterp (*IGDALGetRasterColorInterpretation)(GDALRasterBandH);
typedef int (*IGDALGetGCPCountFunc)(GDALDatasetH);
typedef const char* (*IGDALGetGCPProjectionFunc)(GDALDatasetH);
typedef const GDAL_GCP* (*IGDALGetGCPsFunc)(GDALDatasetH);


typedef void (*IGDALSetColorEntry)(GDALColorTableH,int,const GDALColorEntry * );
typedef GDALColorEntry *(*IGDALGetColorEntry)(GDALColorTableH,int );
typedef CPLErr (*IGDALSetRasterColorTable)(	GDALRasterBandH,GDALColorTableH );
typedef GDALColorTableH (*IGDALGetRasterColorTable)(GDALRasterBandH);
typedef GDALColorTableH (*IGDALCreateColorTable)(GDALPaletteInterp);
typedef int (*IGDALGetColorEntryCount)(GDALColorTableH);
typedef GDALPaletteInterp (*IGDALGetPaletteInterpretation)(GDALColorTableH) 	;
typedef  CPLErr (*IGDALSetRasterColorInterpretation)(GDALRasterBandH,GDALColorInterp)	;

typedef CPLErr (*IGDALSetRasterNoDataValue)(GDALRasterBandH ,double);
typedef double (*IGDALGetRasterNoDataValue)(GDALRasterBandH ,int * );


typedef double (*IGDALGetRasterScale)	(	GDALRasterBandH , int *);
typedef double (*IGDALGetRasterOffset)	(	GDALRasterBandH,int *);

typedef OGRErr (*IOSRImportFromWkt )(OGRSpatialReferenceH, char **) ;
typedef OGRErr (*IOSRSetWellKnownGeogCS )(OGRSpatialReferenceH, char *) ;
typedef int (*IOSRIsProjectedFunc )(OGRSpatialReferenceH) ;
typedef const char* (*IOSRGetAttrValue )(OGRSpatialReferenceH, const char *, int) ;
typedef OGRErr (*IOSRImportFromEPSG )(OGRSpatialReferenceH, int) ;
typedef OGRErr (*IOSRExportToPrettyWkt )(OGRSpatialReferenceH, char **,int) ;
typedef double (*IOSRGetProjParm )(OGRSpatialReferenceH, const char *,double ,OGRErr *) ;
typedef const char* (*IOSRGetAuthorityCode)( OGRSpatialReferenceH, const char *);
typedef const char* (*IOSRGetAuthorityName)( OGRSpatialReferenceH, const char*);

typedef OGRDataSourceH (*IOGROpen )(const char *, int, OGRSFDriverH *) ;
typedef void (*IOGRRegisterAll )() ;
typedef int (*IOGRGetDriverCount )() ;
typedef OGRSFDriverH (*IOGRGetDriver )(int) ;
typedef const char * (*IOGRGetDriverName )(OGRSFDriverH) ;
typedef OGRSFDriverH  (*IOGRGetDriverByName )(const char *) ;
typedef int (*IOGRTestDriverCapability)(OGRSFDriverH ,const char *);
typedef OGRLayerH (*IOGR_DS_GetLayerByName )(OGRDataSourceH, const char *) ;
typedef int (*IGetLayerCount )(OGRDataSourceH) ;
typedef OGRLayerH (*IGetLayer )(OGRDataSourceH, int) ;
typedef const char * (*IGetLayerName )(OGRLayerH) ;
typedef OGRwkbGeometryType (*IGetLayerGeometryType )(OGRLayerH ) ;
typedef void (*IResetReading )(OGRLayerH) ;
typedef OGRFeatureH (*IGetNextFeature )(OGRLayerH ) ;
typedef OGRFeatureDefnH (*IGetLayerDefn )(OGRLayerH ) ;
typedef int (*IGetFieldCount )( OGRFeatureDefnH )  ;
typedef OGRFieldDefnH (*IGetFieldDefn )(OGRFeatureDefnH,int ) ;
typedef OGRFieldType (*IGetFieldType )(OGRFieldDefnH 	hDefn ) ;
typedef int (*IGetFieldAsInteger )(OGRFeatureH,int) ;
typedef double (*IGetFieldAsDouble )(OGRFeatureH,int) ;
typedef const char* (*IGetFieldAsString )(OGRFeatureH,int) ;
typedef int (*IOGR_F_GetFieldAsDateTime) (OGRFeatureH, int, int *, int *, int *, int *, int *, int *, int *) ;
typedef OGRGeometryH (*IGetGeometryRef )(OGRFeatureH ) ;
typedef OGRwkbGeometryType (*IGetGeometryType )(OGRGeometryH) ;
typedef void (*IDestroyFeature )(OGRFeatureH ) ;
typedef int (*IGetPointCount )(OGRGeometryH) ;
typedef void (*IGetPoints )(OGRGeometryH,int,double *,double *,double *) ;
typedef int (*IGetSubGeometryCount )(OGRGeometryH)  ;
typedef OGRGeometryH (*IGetSubGeometryRef )(OGRGeometryH,int) ;
typedef OGRSpatialReferenceH (*IGetSpatialRef )(OGRLayerH hLayer) ;
typedef OGRErr (*IExportToWkt )(OGRSpatialReferenceH,char **) ;
typedef OGRErr (*IOSRImportFromProj4)(OGRSpatialReferenceH, const char *);
typedef OGRErr (*IOSRExportToProj4)(OGRSpatialReferenceH hSRS, char **);
typedef int (*IGetFeatureCount )(OGRLayerH,int) ;
typedef OGRErr 	(*IGetLayerExtent )(OGRLayerH, OGREnvelope *, int) ;
typedef const char * (*IGetFieldName )(OGRFieldDefnH) ;

typedef void (*ICPLPushFinderLocation )( const char * ) ;
typedef const char* (*ICPLGetLastErrorMsg)();
typedef CPLErrorHandler (*ICPLSetErrorHandler)(	CPLErrorHandler );

typedef OGRErr (*IOGRReleaseDataSource) (OGRDataSourceH);
typedef OGRGeometryH (*IOGRGetSpatialFilter)(OGRLayerH);
typedef void (*IOGRGetEnvelope3D) (OGRGeometryH, OGREnvelope*);
typedef void (*IFree)( void * );
typedef int (*IOGRGetGeomFieldCoun) (OGRFeatureH);
typedef void (*IOGR_DS_Destroy) (OGRDataSourceH);
typedef OGRDataSourceH  (*IOGR_Dr_CreateDataSource)(OGRSFDriverH,const char *,char ** );
typedef OGRLayerH (*IOGR_DS_CreateLayer)(OGRDataSourceH,const char *,OGRSpatialReferenceH,OGRwkbGeometryType,char ** );
typedef OGRFieldDefnH (*IOGR_Fld_Create)(const char *,OGRFieldType);
typedef OGRErr (*IOGR_L_CreateField)(OGRLayerH,OGRFieldDefnH,int);
typedef void (*IOGR_Fld_Destroy)(OGRFieldDefnH);
typedef OGRFeatureH (*IOGR_F_Create)(OGRFeatureDefnH);
typedef OGRFeatureDefnH  (*IOGR_L_GetLayerDefn)(OGRLayerH);
typedef int (*IOGR_F_GetFieldIndex)(OGRFeatureH,const char *);
typedef void (* IOGR_F_SetFieldDateTime)(OGRFeatureH, int, int, int, int, int, int, int, int);
typedef void (* IOGR_F_SetFieldDouble)(OGRFeatureH,int, double);
typedef void (* IOGR_F_SetFieldInteger)(OGRFeatureH,int, int);
typedef void (* IOGR_F_SetFieldString)(OGRFeatureH,int, const char *);
typedef OGRGeometryH (*IOGR_G_CreateGeometry)(OGRwkbGeometryType);
typedef void (*IOGR_G_SetPoint)(	OGRGeometryH, int,double,double,double);
typedef void (*IOGR_G_SetPoint_2D)(	OGRGeometryH, int,double,double);
typedef void (*IOGR_G_AddPoint)(	OGRGeometryH, double,double,double);
typedef void (*IOGR_G_AddPoint_2D)(	OGRGeometryH, double,double);
typedef OGRErr (*IOGR_F_SetGeometry)(OGRFeatureH, OGRGeometryH)	;
typedef OGRErr (*IOGR_F_SetGeometryDirectly)(OGRFeatureH, OGRGeometryH)	;
typedef void (*IOGR_G_DestroyGeometry)(	OGRGeometryH);
typedef OGRErr (*IOGR_L_CreateFeature)(OGRLayerH,OGRFeatureH);
typedef OGRErr (*IOGR_G_AddGeometry)(OGRGeometryH,OGRGeometryH);
typedef OGRErr (*IOGR_G_AddGeometryDirectly)(OGRGeometryH,OGRGeometryH);
typedef int (*IOGR_G_GetCoordinateDimension)(OGRGeometryH);
typedef void (*IOSRRelease)( OGRSpatialReferenceH );
typedef OGRSFDriverH (*IOGR_DS_GetDriver)(OGRDataSourceH);
typedef const char * (*IOGR_Dr_GetName)(OGRSFDriverH);


typedef FILE* (*IVSIFileFromMemBuffer)( const char *, GByte *, vsi_l_offset ,int  );
typedef int (*IVSIFCloseL)( FILE * );


class GdalHandle {
    friend class GDALProxy;
    public:
        enum GdalHandleType{
            etGDALDatasetH,
            etOGRDataSourceH
        };

        GdalHandle(void* h, GdalHandleType t, quint64 o=i64UNDEF);
        ~GdalHandle();
        GdalHandleType type();
        void* handle();
    private:
        void* _handle;
        GdalHandleType _type;
        quint64 _owner;
};

class GDALProxy {

    friend GDALProxy* gdal();

    public:
        GDALProxy();
        ~GDALProxy();

        bool isValid() const;
        QStringList getExtensions(IlwisTypes tp) const;
        bool supports(const Resource& resource) const;

        template<class T> T  add(const QString& name) {
            if (!_isValid)
                return 0;

            T fp =  (T )(_libgdal.resolve(name.toLocal8Bit()));
            if ( fp == 0) {
                kernel()->issues()->log(TR("GDAL-proxy not properly initialized; can't find %1").arg(name));
                _isValid = false;
            }
            return fp;
        }

        GdalHandle* openFile(const QFileInfo &filename, quint64 asker, GDALAccess mode=GA_ReadOnly, bool message=true);
        GdalHandle* openUrl(const QUrl &url, quint64 asker, GDALAccess mode=GA_ReadOnly, bool message=true);
        void closeFile(const QString& filename, quint64 asker);
        OGRSpatialReferenceH srsHandle(GdalHandle* handle, const QString& source, bool message=true);
        void releaseSrsHandle(GdalHandle* handle, OGRSpatialReferenceH srshandle, const QString& source);
        Envelope envelope(GdalHandle *handle, int index, bool force=false);

        static QString translateOGRERR(char ogrErrCode);
        static IlwisTypes translateOGRType(OGRwkbGeometryType type) ;

    private:
        bool prepare();

        static QString translateCPLE(int errCode);
        static void cplErrorHandler(CPLErr eErrClass, int err_no, const char *msg);
        static void cplDummyHandler(CPLErr, int, const char *);

        QLibrary _libgdal;
        bool _isValid;
        static GDALProxy *_proxy;
        //next members are stored for efficiency
        QStringList _rasterExtensions;
        QStringList _featureExtensions;
        QStringList _allExtensions;

        QHash<QString, GdalHandle*> _openedDatasets;

 public:
        IGDALClose close;
        IGDALOpen open;
        IGDALAllRegister registerAll;
        IGDALIdentifyDriver identifyDriver;
        IGDALGetSize xsize;
        IGDALGetSize ysize;
        IGDALGetSize layerCount;
        IGDALRasValue minValue;
        IGDALRasValue maxValue;
        IGDALSetRasterNoDataValue setUndefinedValue;
        IGDALGetRasterNoDataValue getUndefinedValue;

        IGDALGetRasterBand getRasterBand;
        IGDALCreate create;
        IGDALGetRasterDataType rasterDataType;
        IGDALGetProjectionRef getProjectionRef;
        IGDALSetProjection setProjection;
        IGDALGetGeoTransform getGeotransform;
        IGDALSetGeoTransform setGeoTransform;
        IGDALRasterIO rasterIO;
        IGDALGetDataTypeSize getDataTypeSize;
        IGDALGetAccess getAccess;
        IGDALGetDriver getDriver;
        IGDALGetDriverByName getGDALDriverByName;
        IGDALGetDatasetDriver getDriverByDataSet;
        IGDALGetDriverCount getDriverCount;
        IGDALGetDriverName getLongName;
        IGDALGetDriverName getShortName;
        IGDALGetMetadataItem getMetaDataItem;
        IGDALGetMetadata getMetaData;
        IGDALGetGCPCountFunc getGCPCount;
        IGDALGetGCPProjectionFunc getGCPProjection;
        IGDALGetGCPsFunc getGCPs;

        IGDALGetRasterColorInterpretation colorInterpretation;
        IGDALSetRasterColorInterpretation setColorInterpretation;
        IGDALSetColorEntry setColorPaletteEntry;
        IGDALGetColorEntry getColorPaletteEntry;
        IGDALSetRasterColorTable setColorPalette;
        IGDALGetRasterColorTable getColorPalette;
        IGDALCreateColorTable createColorPalette;
        IGDALGetColorEntryCount getColorPaletteSize;
        IGDALGetPaletteInterpretation getPaletteColorInterpretation;

        IGDALGetRasterOffset getRasterOffset;
        IGDALGetRasterScale getRasterScale;

        //Open Spatial Reference
        IOSRNewSpatialReference newSpatialRef;
        IOSRImportFromWkt importFromWkt;
        IOSRSetWellKnownGeogCS setWellKnownGeogCs;
        IOSRIsProjectedFunc isProjected;
        IOSRGetAttrValue getAttributeValue;
        IOSRImportFromEPSG importFromEpsg;
        IOSRExportToPrettyWkt exportToPrettyWkt;
        IOSRGetProjParm getProjectionParm;
        IOSRGetAuthorityCode getAuthorityCode;
        IOSRGetAuthorityName getAuthorityName;
        IOSRImportFromProj4 importFromProj4;
        IOSRExportToProj4 export2Proj4;
        IOSRRelease releaseSRS;
        //OGR
        IOGROpen ogrOpen;
        IOGRRegisterAll ogrRegisterAll;
        IOGRGetDriverCount ogrDriverCount;
        IOGRGetDriver ogrGetDriver;
        IOGRGetDriverByName getDriverByName;
        //OGR Driver
        IOGRGetDriverName getOGRDriverName;
        IOGRTestDriverCapability testDriverCapability;
        IOGR_Dr_CreateDataSource createDatasource;

        IOGR_DS_GetLayerByName getLayerByName;
        IGetLayerCount getLayerCount;
        IGetLayer getLayer;
        IGetLayerName getLayerName;
        IGetLayerGeometryType getLayerGeometry;
        IResetReading resetReading;
        IGetNextFeature getNextFeature;
        IGetLayerDefn getLayerDef;
        IGetFieldCount getFieldCount;
        IGetFieldDefn getFieldDfn;
        IGetFieldType getFieldType;
        IGetFieldAsInteger getFieldAsInt;
        IGetFieldAsDouble getFieldAsDouble;
        IGetFieldAsString getFieldAsString;
        IOGR_F_GetFieldAsDateTime getFieldAsDateTime;
        IGetGeometryRef getGeometryRef;
        IGetGeometryType getGeometryType;
        IDestroyFeature destroyFeature;
        IGetPointCount getPointCount;
        IGetPoints getPoint;
        IGetSubGeometryCount getSubGeometryCount;
        IGetSubGeometryRef getSubGeometryRef;
        IGetSpatialRef getSpatialRef;
        IExportToWkt exportToWkt;
        IGetFeatureCount getFeatureCount;
        IGetLayerExtent getLayerExtent;
        IGetFieldName getFieldName;

        ICPLPushFinderLocation pushFinderLocation;
        ICPLGetLastErrorMsg getLastErrorMsg;
        ICPLSetErrorHandler setCPLErrorHandler;


        IOGRReleaseDataSource releaseDataSource;
        IOGRGetSpatialFilter getSpatialFilter;
        IOGRGetEnvelope3D getEnvelope3D;
        IOGR_DS_Destroy destroyDataSource;
        //OGR DataSource
        IOGR_DS_CreateLayer createOgrLayer;
        IOGR_Fld_Create createAttributeDefintion;
        IOGR_L_CreateField addAttribute;
        IOGR_Fld_Destroy destroyAttributeDefintion;
        IOGR_F_Create createFeature;
        IOGR_L_GetLayerDefn getLayerSchema;
        IOGR_F_GetFieldIndex getFieldIndex;
        IOGR_F_SetFieldDateTime setDateTimeAttribute;
        IOGR_F_SetFieldString setStringAttribute;
        IOGR_F_SetFieldInteger setIntegerAttribute;
        IOGR_F_SetFieldDouble setDoubleAttribute;
        IOGR_G_CreateGeometry createGeometry;
        IOGR_G_AddPoint addPoint;
        IOGR_G_AddPoint_2D addPoint2D;
        IOGR_G_SetPoint setPoint;
        IOGR_G_SetPoint_2D setPoint2D;
        IOGR_F_SetGeometry setGeometry;
        IOGR_F_SetGeometryDirectly setGeometryDirectly;
        IOGR_G_DestroyGeometry destroyGeometry;
        IOGR_L_CreateFeature addFeature2Layer;
        IOGR_G_AddGeometry addGeometry;
        IOGR_G_AddGeometryDirectly addGeometryDirectly;
        IOGR_G_GetCoordinateDimension getCoordinateDimension;
        IOGR_Dr_GetName getDriverName;
        IOGR_DS_GetDriver getDriverFromDS;

        IVSIFileFromMemBuffer vsiFileFromMem;
        IVSIFCloseL vsiClose;
        IFree free;
};

GDALProxy *gdal();
}

}

#endif // GDALPROXY_H
