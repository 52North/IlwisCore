#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QDir>

#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "domain.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "gdalproxy.h"
#include "dataformat.h"
#include "gdalconnector.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "eigen3/Eigen/LU"
#include "mathhelper.h"
#include "controlpoint.h"
#include "ctpgeoreference.h"
#include "planarctpgeoreference.h"
#include "georefconnector.h"


using namespace Ilwis;
using namespace Gdal;

ConnectorInterface *GeorefConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options) {
    return new GeorefConnector(resource,load, options);
}

GeorefConnector::GeorefConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : GdalConnector(resource, load, options)
{
}

bool GeorefConnector::loadMetaData(IlwisObject *data, const IOOptions &options){
    if(!GdalConnector::loadMetaData(data, options))
        return false;

    //TODO: tiepoints grf

    GeoReference * grf = static_cast<GeoReference *>(data);

    ICoordinateSystem csy = setObject<ICoordinateSystem>("coordinatesystem", _fileUrl);
    if(!csy.isValid()) {
        QString resource = QString("code=csy:unknown");
        if (!csy.prepare(resource)) {
            kernel()->issues()->log(TR("Fallback to 'unknown' failed, corrupt system files defintion"));
            return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "coordinatesystem", grf->name());
        }
    }

    if(_handle->type() == GdalHandle::etGDALDatasetH){
        Coordinate cMin, cMax;
        Size<> sz(gdal()->xsize(_handle->handle()), gdal()->ysize(_handle->handle()), gdal()->layerCount(_handle->handle()));
        double geosys[6];
        CPLErr err = gdal()->getGeotransform(_handle->handle(), geosys) ;
        if ( err == CE_None) {
            grf->create("corners");
            double a1 = geosys[0];
            double b1 = geosys[3];
            double a2 = geosys[1];
            double b2 = geosys[5];
            Coordinate crdLeftup( a1 , b1);
            Coordinate crdRightDown(a1 + sz.xsize() * a2, b1 + sz.ysize() * b2 ) ;
            cMin = Coordinate( min(crdLeftup.x, crdRightDown.x), min(crdLeftup.y, crdRightDown.y));
            cMax = Coordinate( max(crdLeftup.x, crdRightDown.x), max(crdLeftup.y, crdRightDown.y));
        } else {
            int iNrTiePoints = gdal()->getGCPCount(_handle->handle());
            if (iNrTiePoints > 0) {
                grf->create("tiepoints");
                const GDAL_GCP* amtp = gdal()->getGCPs(_handle->handle());
                QSharedPointer<PlanarCTPGeoReference> grfctp(grf->as<PlanarCTPGeoReference>());
                Envelope envTieLimits;
                for (int i = 0; i < iNrTiePoints; i++) {
                    Coordinate crdtiep(amtp[i].dfGCPX,amtp[i].dfGCPY,0);
                    ControlPoint ctp(crdtiep,Pixeld(amtp[i].dfGCPPixel,amtp[i].dfGCPLine));
                    ctp.active(true);
                    grfctp->setControlPoint(ctp);
                    envTieLimits += crdtiep;
                }
                cMin = envTieLimits.min_corner();
                cMax = envTieLimits.max_corner();
            } else {
                grf->create("corners"); // let GDAL fallback to "corners"?
                cMin = Coordinate( 0, 0 );
                cMax = Coordinate( sz.xsize() - 1, sz.ysize() - 1);
            }
        }
        grf->coordinateSystem(csy);
        grf->size(sz);
        grf->envelope(Envelope(cMin, cMax));
        grf->compute();
    }else{
        return ERROR2(ERR_INVALID_PROPERTY_FOR_2, "Georeference", grf->name());
    }

    return true;
}

IlwisObject *GeorefConnector::create() const{
    return new GeoReference(_resource);
}


