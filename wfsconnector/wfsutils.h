#ifndef WFSUTILS_H
#define WFSUTILS_H

#include <QCoreApplication>

#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "feature.h"
#include "coverage.h"
#include "featurecoverage.h"

namespace Ilwis {
namespace Wfs {

class WfsUtils
{

public:
    static bool isValidWfsUrl(QUrl url)
    {
        QUrlQuery query(url);
        lowerCaseKeys(query);

        bool isHttpRequest = url.scheme().startsWith("http");
        bool isWfsRequest = isExpectedValue(query.queryItemValue("service"), "wfs");
        if(isHttpRequest && isWfsRequest) checkVersionCompatibility(query.queryItemValue("acceptversions"));
        return isHttpRequest && isWfsRequest;
    }

    static void lowerCaseKeys(QUrlQuery &query)
    {
        for (QPair<QString,QString> kvm : query.queryItems()) {
            query.removeQueryItem(kvm.first);
            query.addQueryItem(kvm.first.toLower(), kvm.second);
        }
    }

    static QString normalizeEpsgCode(QString epsgCode) {
        QString normalizedCode("epsg:");
        int splitIndex = epsgCode.lastIndexOf(":");
        if (splitIndex == -1) {
            splitIndex = epsgCode.lastIndexOf("/");
        }
        QString code = epsgCode.mid(splitIndex + 1);
        return normalizedCode.append(code.trimmed());
    }

    static QString getInternalNameFrom(QString name, quint64 id) {
        return QString(INTERNAL_CATALOG + "/%1_%2").arg(name).arg(id);
    }


    static void addSpatialMetadata(FeatureCoverage *fcoverage, Resource resource)
    {
        ICoordinateSystem crs;
        QString res = resource["coordinatesystem"].toString();
        if (crs.prepare(res, itCONVENTIONALCOORDSYSTEM)) {
            fcoverage->coordinateSystem(crs);
        } else {
            ERROR1("Could not prepare crs with %1.", res);
        }

        Coordinate ll = createCoordinateFromWgs84LatLon(resource["envelope.ll"].toString());
        Coordinate ur = createCoordinateFromWgs84LatLon(resource["envelope.ur"].toString());
        Envelope envelope(ll, ur);
        fcoverage->envelope(envelope);
    }

    static Coordinate createCoordinateFromWgs84LatLon(QString latlon)
    {
        int splitIndex = latlon.indexOf(" ");
        QString lon = latlon.left(splitIndex).trimmed();
        QString lat = latlon.mid(splitIndex + 1).trimmed();
        Coordinate coords(lon.toDouble(), lat.toDouble());
        return coords;
    }

    static IlwisTypes getGeometryType(const QString &gmlName)
    {
        if (isPolygonType(gmlName)) {
            return itPOLYGON;
        }
        if (isLineStringType(gmlName)) {
            return itLINE;
        }
        if (isPointType(gmlName)) {
            return itPOINT;
        }
        return itUNKNOWN;
    }

    static bool isPolygonType(const QString &gmlName)
    {
        bool isPolygon = false;
        isPolygon = isPolygon || gmlName.contains("Polygon");
        isPolygon = isPolygon || gmlName.contains("Surface");
        isPolygon = isPolygon || gmlName.contains("Ring");
        return isPolygon;
    }

    static bool isLineStringType(const QString &gmlName)
    {
        return gmlName.contains("Line")
                || gmlName.contains("Curve");
    }

    static bool isPointType(const QString &gmlName)
    {
        return gmlName.contains("Point");
    }

    static bool swapAxes(const Resource resource, const ICoordinateSystem crs)
    {
        bool latLonOrder = isDefinedAsLatLonAxesOrder(crs);
        bool forceXY = isForcedXYOrder(resource);
        return latLonOrder && forceXY;
    }

    static bool isForcedXYOrder(const Resource resource)
    {
        return !(resource.hasProperty("forceXY")
                 && resource["forceXY"].toBool());
    }

    static bool isDefinedAsLatLonAxesOrder(ICoordinateSystem crs)
    {
        QString sqlBuilder;
        sqlBuilder.append("SELECT code FROM epsgcodeswithlatlonaxesorder ");
        sqlBuilder.append(" WHERE code='");
        sqlBuilder.append(crs->code());
        sqlBuilder.append("' ;");

        QSqlQuery query(kernel()->database());
        query.exec(sqlBuilder);
        // exact one or no match
        return query.next();
    }


private:
    static void checkVersionCompatibility(QString kvpValueVersions) {
        QStringList versions = kvpValueVersions.split(",",QString::SkipEmptyParts);
        if (versions.isEmpty() || !versions.contains("1.1")) {
            WARN("Module supports WFS 1.1.0 only and will set version parameter explicitly on each request!");
        }
    }

    /**
     * @brief WfsCatalogConnector::hasParameterValue checks if a certain parameter value is present.
     * @param actual the actual value to check.
     * @param expected the expected value.
     * @return true if the actual parameter is equal (incasesensitive) to the expected value.
     */
    static bool isExpectedValue(QString actual, QString expected)
    {
        if (expected != "" && actual == "") {
            return false;
        }
        return actual.compare(expected, Qt::CaseInsensitive) == 0;
    }

};

}
}

#endif // WFSUTILS_H
