#include <QMap>
#include <QString>
#include <QXmlStreamAttributes>

#include "geos/io/ParseException.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/Point.h"

#include "kernel.h"
#include "coverage.h"
#include "ilwis.h"
#include "ilwisdata.h"
#include "domain.h"
#include "datadefinition.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "table.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "textdomain.h"
#include "juliantime.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "geometryhelper.h"

#include "xmlstreamparser.h"
#include "wfsparsingcontext.h"
#include "wfsfeatureparser.h"
#include "wfsutils.h"

using namespace Ilwis;
using namespace Wfs;

WfsFeatureParser::WfsFeatureParser(SPWfsResponse response, FeatureCoverage *fcoverage): _fcoverage(fcoverage)
{
    _parser = new XmlStreamParser(response->device());
    _parser->addNamespaceMapping("wfs", "http://www.opengis.net/wfs");
    _parser->addNamespaceMapping("ows", "http://www.opengis.net/ows");
    _parser->addNamespaceMapping("gml", "http://www.opengis.net/gml");
}

WfsFeatureParser::~WfsFeatureParser()
{
    delete _parser;
}

void WfsFeatureParser::context(const WfsParsingContext &context)
{
    QString targetNamespace =  context.namespaceMappings()[""];
    _parser->addTargetNamespaceMapping(targetNamespace);
    _context = context;
}

WfsParsingContext WfsFeatureParser::context() const
{
    return _context;
}

void WfsFeatureParser::parseFeatureMembers()
{
    qDebug() << "WfsFeatureParser::parseFeatureMembers()";

    ITable table = _fcoverage->attributeTable();
    _featureType = "target:" + _context.featureType();

    quint64 featureCount = 0;
    if (_parser->findNextOf( {"wfs:FeatureCollection"} )) {
        if (_parser->findNextOf( {"gml:featureMembers", "gml:featureMember"} )) {
            QString collectiontype = _parser->name();
            qint32 colonIdx = collectiontype.indexOf(":");
            collectiontype = collectiontype.mid(colonIdx + 1);
            while (_parser->findNextOf( {_featureType} )) {
                _context.setCurrentItem(_parser->attributes().value("gml:id").toString());
                std::vector<QVariant> record(table->columnCount());

                if ( !parseFeature(record, table)) {
                    ERROR1("Could not parse feature collection (@gml:id='%1')", _context.currentItem());
                    return;
                }
                table->record(featureCount++, record); // load content
                //qDebug() << "added feature #" << featureCount;
                _parser->moveToEndOf(_featureType);

                if (collectiontype == "gml:featureMember" || collectiontype == "featureMember") {
                    // enter next collection member
                    _parser->readNextStartElement();
                }
            }
        }
    }
    _fcoverage->setAttributes(table);
}

bool WfsFeatureParser::parseFeature(std::vector<QVariant> &record, ITable& table)
{
    bool continueReadingStream = true;
    QString geometryAttributeName = _context.geometryAtttributeName();
    for (int i = 0; i < table->columnCount(); i++) {

        if (_parser->atEnd()) {
            qDebug() << "document parsing finished.";
            break;
        }

        ColumnDefinition& coldef = table->columndefinitionRef(i);

        DataDefinition& datadef = coldef.datadef();
        if( !datadef.domain().isValid()) {
            WARN2(ERR_NO_INITIALIZED_2, "domain", coldef.name());
            record[i] = QVariant(); // empty
            continue;
        }

        if (continueReadingStream) {
            if ( !_parser->readNextStartElement()) {
                break; // end of feature record
            }
        } else {
            // continue next time
            continueReadingStream = true;
        }

        QString currentElementName = _parser->name();
        if (_parser->isAtBeginningOf("gml:boundedBy")) {
            i--; // not written to table

            // TODO: add boundedBy fill function
            _parser->skipCurrentElement(); // ignore for now

            continue;
        }

        // if geometry is in between attributes
        if (currentElementName == geometryAttributeName) {
            i--; // not written to table
            createNewFeature();
            QString qname = "target:" + geometryAttributeName;
            if ( !_parser->moveToEndOf(qname)) {
                qWarning() << "could not find end of " << qname;
                return false;
            }
            continue;
        }

        // check for missing (optional) elements
        if (coldef.name() != currentElementName) {
            record[i] = QVariant(); // empty
            continueReadingStream = false;
            continue; // pause stream
        }

        IlwisTypes tp = datadef.domain()->valueType();

        if (tp & itSTRING){
            record[i] = fillStringColumn();
        } else if (tp & itBOOL){
            record[i] = fillBoolColumn();
        } else if (tp & itDATETIME || tp & itDATE){
            record[i] = fillDateTimeColumn();
        } else if (tp & itINTEGER){
            NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range<NumericRange>()->clone());
            //creating the actual range as invalid to be adjusted in the fillers
            double min = r->min();
            r->min(r->max());
            r->max(min);
            datadef.range(r);
            record[i] = fillIntegerColumn();
        } else if (tp & itDOUBLE){
            //creating the actual range as invalid to be adjusted in the fillers
            NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range<NumericRange>()->clone());
            double min = r->min();
            r->min(r->max());
            r->max(min);
            datadef.range(r);
            record[i] = fillDoubleColumn();
        } else if (tp & itBOOL){
            record[i] = fillBoolColumn();
        } else {
            record[i] = QVariant();
        }
    }

    if (_parser->readNextStartElement()) {
        // geometry is at the end of the attribute list
        if (_parser->name() == geometryAttributeName) {
            createNewFeature();
        }
    }

    //qDebug() << "new feature created.";
    return true;
}

QVariant WfsFeatureParser::fillStringColumn()
{
    return QVariant(_parser->readElementText());
}

QVariant WfsFeatureParser::fillIntegerColumn()
{
    return QVariant(_parser->readElementText().toInt());
}

QVariant WfsFeatureParser::fillDoubleColumn()
{
    return QVariant(_parser->readElementText().toDouble());
}

QVariant WfsFeatureParser::fillBoolColumn()
{
    QString v = _parser->readElementText();
    return v == "true" || v.toInt() != 0
            ? QVariant(true)
            : QVariant(false);
}

QVariant WfsFeatureParser::fillDateTimeColumn()
{
    Time time(_parser->readElementText());
    return IVARIANT(time);
}

void WfsFeatureParser::createNewFeature()
{
    geos::geom::Geometry *geometry = parseFeatureGeometry();
    _fcoverage->newFeature(geometry, false);
}

geos::geom::Geometry *WfsFeatureParser::parseFeatureGeometry()
{
    // move on element
    _parser->readNextStartElement();

    updateSrsInfo();
    QString gmlName = _parser->name();
    bool isMultiGeometry = gmlName.contains("Multi");
    if (WfsUtils::isPolygonType(gmlName)) {
        return createPolygon(isMultiGeometry);
    } else if (WfsUtils::isLineStringType(gmlName)) {
        return createLineString(isMultiGeometry);
    } else if (WfsUtils::isPointType(gmlName)) {
        return createPoint(isMultiGeometry);
    }
    return 0;
}



geos::geom::Geometry *WfsFeatureParser::createPolygon(bool isMultiGeometry)
{
    if (isMultiGeometry) {
        std::vector<geos::geom::Geometry *>* multi = new std::vector<geos::geom::Geometry *>();
        if (updateSrsInfoUntil("gml:surfaceMember")) {
            do {
                bool ok = false;
                geos::geom::Geometry *geometry = parsePolygon(ok);
                if ( !ok) {
                    ERROR0("Could not parse GML MultiSurface member.");
                    return _fcoverage->geomfactory()->createMultiPolygon();
                }
                multi->push_back(geometry);
            } while (_parser->findNextOf( {"gml:surfaceMember"} ));
            return _fcoverage->geomfactory()->createMultiPolygon(multi);
        }
    } else {
        bool ok = false;
        geos::geom::Geometry *geometry = parsePolygon(ok);
        if ( !ok) {
            ERROR0("Could not parse GML Surface.");
        }
        return geometry;
    }
    return 0;
}

geos::geom::Geometry *WfsFeatureParser::createLineString(bool isMultiGeometry)
{
    if (isMultiGeometry) {
        std::vector<geos::geom::Geometry *>* multi = new std::vector<geos::geom::Geometry *>();
        if (updateSrsInfoUntil("gml:curveMember")) {
            do {
                bool ok = false;
                geos::geom::Geometry *geometry = parseLineString(ok);
                if ( !ok) {
                    ERROR0("Could not parse GML MultiCurve member.");
                    return _fcoverage->geomfactory()->createMultiLineString();
                }
                multi->push_back(geometry);
            } while (_parser->findNextOf( { "gml:curveMember" } ));
            return _fcoverage->geomfactory()->createMultiLineString(multi);
        }
    } else {
        bool ok = false;
        updateSrsInfo();
        geos::geom::Geometry *geometry = parseLineString(ok);
        if ( !ok) {
            ERROR0("Could not parse GML Curve.");
        }
        return geometry;
    }
    return 0;
}

geos::geom::Geometry *WfsFeatureParser::createPoint(bool isMultiGeometry)
{
    if (isMultiGeometry) {
        std::vector<geos::geom::Geometry *>* multi = new std::vector<geos::geom::Geometry *>();
        if (updateSrsInfoUntil("gml:pointMember")) {
            do {
                bool ok = false;
                geos::geom::Geometry *geometry = parsePoint(ok);
                if ( !ok) {
                    ERROR0("Could not parse GML MultiPoint member.");
                    return _fcoverage->geomfactory()->createMultiPoint();
                }
                multi->push_back(geometry);
            } while (_parser->findNextOf( {"gml:pointMember"} ));
            return _fcoverage->geomfactory()->createMultiPoint(multi);
        }
    } else {
        bool ok = false;
        updateSrsInfo();
        geos::geom::Geometry *geometry = parsePoint(ok);
        if ( !ok) {
            ERROR0("Could not parse GML Point.");
        }
        return geometry;
    }
    return 0;
}

void WfsFeatureParser::updateSrsInfo()
{
    QXmlStreamAttributes attributes = _parser->attributes();
    QString srs = attributes.value("srsName").toString();
    QString dimension = attributes.value("srsDimension").toString();
    if (!dimension.isEmpty()) _context.setSrsDimension(dimension.toInt());
    if (!srs.isEmpty()) _context.setSrsName(WfsUtils::normalizeEpsgCode(srs));
}

bool WfsFeatureParser::updateSrsInfoUntil(QString qname)
{
    while (!_parser->isAtBeginningOf(qname)) {
        if (_parser->readNextStartElement()) {
            updateSrsInfo();
        } else {
            return false;
        }
    }
    return true;
}

void WfsFeatureParser::initCrs(ICoordinateSystem &crs) {
    QString geomCrsCode = QString("code=csy:").append(_context.srsName());
    if ( !crs.prepare(geomCrsCode, itCONVENTIONALCOORDSYSTEM)) {
        ERROR1("Could not prepare crs with code=%1.",_context.srsName());
    }
    _swapAxesNeededToAlignInternXYOrder = WfsUtils::swapAxes(_fcoverage->resource(), crs);
    if (_swapAxesNeededToAlignInternXYOrder) {
        DEBUG0("XY axes order used internally. OGC respects lat/lon order, though.");
        DEBUG1("Will swap axes order for CRS '%1'.", _context.srsName());
    }
}

geos::geom::Point *WfsFeatureParser::parsePoint(bool &ok)
{
    try {
        ICoordinateSystem crs;
        if (_parser->findNextOf( { "gml:pos"} )) {
            ok = true;
            updateSrsInfo();
            initCrs(crs);
            QString wkt;
            gmlPosListToWktLineString(wkt, _parser->readElementText());
            if (wkt.isEmpty()) {
                WARN1("Parsed empty geometry at feature '%1'", _context.currentItem());
                return _fcoverage->geomfactory()->createPoint();
            } else {
                geos::geom::Geometry *geometry = GeometryHelper::fromWKT(wkt, crs);
                return _fcoverage->geomfactory()->createPoint(geometry->getCoordinates());
            }
        }
        ERROR0("Could not find gml:pos");
    } catch(std::exception &e) {
        ERROR1("Could not parse WKT Point %1", e.what());
    }

    ok = false; // return empty point
    return _fcoverage->geomfactory()->createPoint();
}

geos::geom::LineString *WfsFeatureParser::parseLineString(bool &ok)
{
    try {
        ICoordinateSystem crs;
        if (_parser->findNextOf( {"gml:posList", "gml:pos"} )) {
            ok = true;
            updateSrsInfo();
            initCrs(crs);
            QString wkt;
            gmlPosListToWktLineString(wkt, _parser->readElementText());
            if (wkt.isEmpty()) {
                WARN1("Parsed empty geometry at feature '%1'", _context.currentItem());
                return _fcoverage->geomfactory()->createLineString();
            } else {
                geos::geom::Geometry *geometry = GeometryHelper::fromWKT(wkt, crs);
                return _fcoverage->geomfactory()->createLineString(geometry->getCoordinates());
            }
        }
        ERROR0("Could not neither find gml:posList nor gml:pos");
    } catch(std::exception &e) {
        ERROR1("Could not parse WKT LineString %1", e.what());
    }

    ok = false; // return empty line string
    return _fcoverage->geomfactory()->createLineString();
}

geos::geom::Polygon * WfsFeatureParser::parsePolygon(bool &ok)
{
    try {
        ok = true;
        geos::geom::LinearRing *outer = parseExteriorRing();
        std::vector<geos::geom::Geometry *> *inners = parseInteriorRings();
        return _fcoverage->geomfactory()->createPolygon(outer, inners);
    } catch(std::exception &e) {
        ERROR1("Could not parse WKT Polygon %1", e.what());
    }

    ok = false; // return empty polygon
    return _fcoverage->geomfactory()->createPolygon();
}

geos::geom::LinearRing *WfsFeatureParser::parseExteriorRing()
{
    geos::geom::LinearRing *ring;

    ICoordinateSystem crs;
    if (_parser->findNextOf( {"gml:exterior"} )) {
        if (_parser->findNextOf( {"gml:posList"} )) {
            initCrs(crs);
            QString wkt;
            gmlPosListToWktPolygon(wkt, _parser->readElementText());
            if (wkt.isEmpty()) {
                WARN1("Parsed empty geometry at feature '%1'", _context.currentItem());
                ring = _fcoverage->geomfactory()->createLinearRing();
            } else {
                geos::geom::Geometry *geometry = GeometryHelper::fromWKT(wkt, crs);
                ring = _fcoverage->geomfactory()->createLinearRing(geometry->getCoordinates());
            }
            _parser->moveToEndOf("gml:exterior");
        }
    }
    return ring;
}


std::vector<geos::geom::Geometry *> *WfsFeatureParser::parseInteriorRings()
{
    std::vector<geos::geom::Geometry *>* innerRings = new std::vector<geos::geom::Geometry *>();

    do {
        ICoordinateSystem crs;
        if (_parser->findNextOf( { "gml:posList" })) {
            initCrs(crs);
            QString wkt;
            gmlPosListToWktPolygon(wkt, _parser->readElementText());
            if ( !wkt.isEmpty()) {
                geos::geom::LinearRing *ring;
                geos::geom::Geometry *geometry = GeometryHelper::fromWKT(wkt, crs);
                ring = _fcoverage->geomfactory()->createLinearRing(geometry->getCoordinates());
                innerRings->push_back(ring);
            } else {
                WARN1("Parsed empty geometry at feature '%1'", _context.currentItem());
            }
            _parser->moveToEndOf("gml:interior");
        }
    } while (_parser->findNextOf( {"gml:interior"} ));
    return innerRings;
}

QString WfsFeatureParser::gmlPosListToWktCoords(QString gmlPosList)
{
    QString wktCoords;
    QStringList coords = gmlPosList.split(" ");
    if (_context.srsDimension() == 2) {
        for (int i = 0; i < coords.size() - 1; i++) {
            QString first = coords.at(i);
            QString second = coords.at(++i);
            if (!_swapAxesNeededToAlignInternXYOrder) {
                wktCoords.append(first).append(" ");
                wktCoords.append(second).append(" ");
            } else {
                wktCoords.append(second).append(" ");
                wktCoords.append(first).append(" ");
            }
            wktCoords.append(", ");
        }
    } else if (_context.srsDimension() == 3) {
        for (int i = 0; i < coords.size() - 2; i++) {
            QString first = coords.at(i);
            QString second = coords.at(++i);
            QString third = coords.at(++i);
            if (!_swapAxesNeededToAlignInternXYOrder) {
                wktCoords.append(first).append(" ");
                wktCoords.append(second).append(" ");
            } else {
                wktCoords.append(second).append(" ");
                wktCoords.append(first).append(" ");
            }
            wktCoords.append(third);
            wktCoords.append(", ");
        }
    }
    return wktCoords.left(wktCoords.lastIndexOf(","));
}

void WfsFeatureParser::gmlPosListToWktPolygon(QString &wkt, QString gmlPosList)
{
    if ( !gmlPosList.isEmpty()) {
        wkt.append("POLYGON((");
        wkt.append(gmlPosListToWktCoords(gmlPosList));
        wkt.append("))");
    }
}

void WfsFeatureParser::gmlPosListToWktLineString(QString &wkt, QString gmlPosList)
{
    if ( !gmlPosList.isEmpty()) {
        wkt.append("LINESTRING(");
        wkt.append(gmlPosListToWktCoords(gmlPosList));
        wkt.append(")");
    }
}

void WfsFeatureParser::gmlPosListToWktPoint(QString &wkt, QString gmlPosList)
{
    if ( !gmlPosList.isEmpty()) {
        wkt.append("POINT(");
        wkt.append(gmlPosListToWktCoords(gmlPosList));
        wkt.append(")");
    }
}


