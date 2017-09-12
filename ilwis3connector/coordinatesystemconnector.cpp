#include <QUrl>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QStringList>

#include "kernel.h"
#include "geometries.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "ilwisdata.h"
#include "ilwis3connector.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "coordinatesystem.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "boundsonlycoordinatesystem.h"
#include "coordinatesystemconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

ConnectorInterface *CoordinateSystemConnector::create(const Resource &resource,bool load,const IOOptions& options) {
    return new CoordinateSystemConnector(resource, load, options);

}

CoordinateSystemConnector::CoordinateSystemConnector(const Resource &resource, bool load, const IOOptions &options) : Ilwis3Connector(resource, load, options)
{
    QString type = _odf->value("CoordSystem","Type");
}

IlwisObject *CoordinateSystemConnector::create() const {
    IlwisObject *object = 0;
    if ( type() == itCONVENTIONALCOORDSYSTEM)
        object = new ConventionalCoordinateSystem(_resource);
    if ( type() == itBOUNDSONLYCSY)
        object = new BoundsOnlyCoordinateSystem(_resource);

    return object;
}

bool CoordinateSystemConnector::loadMetaData(IlwisObject* data, const IOOptions& options)
{
    Ilwis3Connector::loadMetaData(data, options);
    CoordinateSystem *csy = static_cast<CoordinateSystem *>(data);

    IEllipsoid ell = getEllipsoid();
    GeodeticDatum *datum = getDatum(ell);

    QString cb = _odf->value("CoordSystem", "CoordBounds");
    QStringList cbparts = cb.split(" ");
    if ( cbparts.size() == 4 && cbparts[0] != "-1e+308") {
        bool ok1, ok2, ok3, ok4;
        Envelope box( Coordinate(
                                cbparts[0].toDouble(&ok1),
                                cbparts[1].toDouble(&ok2)),
                           Coordinate(
                                cbparts[2].toDouble(&ok3),
                                cbparts[3].toDouble(&ok4)));
        if ( !( ok1 && ok2 && ok3 && ok4)) {
            return ERROR2(ERR_NO_INITIALIZED_2, TR("envelop"), csy->name());
        }
        csy->envelope(box);
    } else {
        QString type = _odf->value("CoordSystem", "Type");
        if ( type == "LatLon") {
            Envelope box(Coordinate(-180,-90), Coordinate(180,90));
            csy->envelope(box);
        }
    }

    if ( type() == itCONVENTIONALCOORDSYSTEM ) {
        ConventionalCoordinateSystem *csycc = static_cast<ConventionalCoordinateSystem *>(csy);
        IProjection proj = getProjection(csycc);
        if ( !proj.isValid()) {
            return ERROR1(ERR_NO_INITIALIZED_1, "projection");
        }
        csycc->setDatum(datum);
        csycc->setEllipsoid(ell);
        csycc->setProjection(proj);
        proj->setCoordinateSystem(csycc);


        proj->setParameter(Projection::pvELLCODE, ell->toProj4());
        csycc->prepare();
    } else if ( type() == itUNKNOWN){
        //TODO: other types of csy
    }
    return true;
}

bool CoordinateSystemConnector::canUse(const Resource& resource,const UPCatalogConnector &container) // static
{
    IlwisTypes requiredType = resource.ilwisType();
    if ( (requiredType & itCOORDSYSTEM)!= 0)
        return true;
    QString file = resource.url(true).toLocalFile();
    QFileInfo inf(file);
    if ( inf.exists()) {
        IniFile odf;
        odf.setIniFile(inf);
        QString t = odf.value("CoordSystem","Type");
        if ( t == sUNDEF ) {
            if ( odf.value("CoordSystem","Projection") != "") // necessary due to incompleteness of ilwis odf's
                t = "Projection";
        }
        if (t == "LatLon" && requiredType == itCONVENTIONALCOORDSYSTEM)
            return true;
        else if ( t == "Projection" && requiredType == itCONVENTIONALCOORDSYSTEM)
            return true;
        if ( t == "BoundsOnly" && requiredType == itBOUNDSONLYCSY)
            return true;

    }
    return false;
}

IEllipsoid CoordinateSystemConnector::getEllipsoid() {
    QString ell = _odf->value("CoordSystem","Ellipsoid");
    if ( ell == "?")
        return IEllipsoid();
    if ( ell.toLower() == "user defined") {
        double invf = _odf->value("Ellipsoid","1/f").toDouble();
        double majoraxis = _odf->value("Ellipsoid","a").toDouble();
        IEllipsoid ellipsoid;
        ellipsoid.prepare();
        ellipsoid->setEllipsoid(majoraxis,invf);
        return ellipsoid;
    }
    IEllipsoid ellipsoid;
    QString code = name2Code(ell, "ellipsoid");
    if ( code == sUNDEF)
        return IEllipsoid();

    QString resource = QString("ilwis://system/ellipsoids/%1").arg(code);

    ellipsoid.prepare(resource);

    return ellipsoid;
}

QString CoordinateSystemConnector::prjParam2IlwisName(Projection::ProjectionParamValue parm)
{
    switch(parm){
        case Projection::pvAZIMCLINE:
        return "Azim of Central Line of True Scale";
    case Projection::pvAZIMYAXIS:
        return "Azim of Projection Y-Axis";
    case Projection::pvHEIGHT:
        return "Height Persp. Center";
    case Projection::pvSCALE:
        return "Scale Factor";
    case Projection::pvCENTRALPARALLEL:
        return "Central Parallel";
    case Projection::pvSTANDARDPARALLEL1:
        return "Standard Parallel 1";
    case Projection::pvSTANDARDPARALLEL2:
        return "Standard Parallel 2";
    case Projection::pvLATITUDEOFTRUESCALE:
        return "Latitude of True Scale";
    case Projection::pvCENTRALMERIDIAN:
        return "Central Meridian";
    case Projection::pvNORIENTED:
        return "North Oriented XY Coord System";
    case Projection::pvNORTH:
        return "Northern Hemisphere";
    case Projection::pvPOLE:
        return "Pole of Oblique Cylinder";
    case Projection::pvTILTED:
        return "Tilted/Rotated Projection Plane";
    case Projection::pvTILT:
        return "Tilt of Projection Plane";
    case Projection::pvFALSEEASTING:
        return "False Easting";
    case Projection::pvFALSENORTHING:
        return "False Northing";
    case Projection::pvZONE:
        return "Zone";
    default:
        return sUNDEF;
    }
    return sUNDEF;
}

GeodeticDatum *CoordinateSystemConnector::getDatum(IEllipsoid& ell) {
    QString datum =_odf->value("CoordSystem","Datum");
    if ( datum == sUNDEF)
        return 0; // not an error; simply no datum with this csy

    QString area = _odf->value("CoordSystem","Datum Area");
    if ( area != sUNDEF && area != "" )
        datum = datum + "|" + area;
    QString code = name2Code(datum,"datum");

    if ( code == "?"){
        QString dx = _odf->value("Datum", "dx");
        if ( dx == sUNDEF){
            kernel()->issues()->log(TR("No datum code for this alias %1").arg(datum));
            return 0;
        }
        QString dy = _odf->value("Datum", "dy");
        QString dz = _odf->value("Datum", "dz");
        GeodeticDatum *gdata = new GeodeticDatum();
        gdata->setArea(sUNDEF);
        gdata->code("User defined");
        gdata->set3TransformationParameters(dx.toDouble(), dy.toDouble(), dz.toDouble(), ell);

        return gdata;
    }

    InternalDatabaseConnection stmt;
    QString query = QString("Select * from datum where code='%1'").arg(code);

    if (stmt.exec(query)) {
        if ( stmt.next()) {
            GeodeticDatum *gdata = new GeodeticDatum();
            QString area = stmt.value(stmt.record().indexOf("area")).toString();
            QString code = stmt.value(stmt.record().indexOf("code")).toString();
            double dx = stmt.value(stmt.record().indexOf("dx")).toDouble();
            double dy = stmt.value(stmt.record().indexOf("dy")).toDouble();
            double dz = stmt.value(stmt.record().indexOf("dz")).toDouble();
            gdata->setArea(area);
            gdata->code(code);
            QString ellipsoid = stmt.value(stmt.record().indexOf("ellipsoid")).toString();
            if ( !ell.isValid() && ellipsoid != sUNDEF){
               QString ellres = QString("code=ellipsoid:%1").arg(ellipsoid);
               if (!ell.prepare(ellres)) {
                    kernel()->issues()->log(TR("No ellipsoid for this code %1").arg(ellipsoid),IssueObject::itWarning);
               }
            }

            gdata->set3TransformationParameters(dx, dy, dz, ell);

            return gdata;

        } else {
            kernel()->issues()->log(TR("No datum for this code %1").arg(code));
        }
    } else {
        kernel()->issues()->logSql(stmt.lastError());
    }
    return 0;
}

IProjection CoordinateSystemConnector::getProjection(ConventionalCoordinateSystem *csycc) {
    QString projection = _odf->value("CoordSystem","Projection");
    if (projection == "?") {
        QString type = _odf->value("CoordSystem","Type");
        if ( type == "LatLon") {
            projection = type;
            GeodeticDatum *gdata = new GeodeticDatum();
            gdata->fromCode("DWGS84");
            csycc->setDatum(gdata);
        } else
            return IProjection();
    }

    QString code = name2Code(projection, "projection");
    Resource resource(QUrl(QString("ilwis://system/projections/%1").arg(code)), itPROJECTION);

    if ( code == sUNDEF) {
        kernel()->issues()->log(TR("Couldnt find projection %1").arg(projection));
        return IProjection();
    }
    resource.code(code);

    IProjection proj;
    if(!proj.prepare(resource))
        return IProjection();

    bool ok;
    double falseEasting = _odf->value("Projection","False Easting").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvFALSEEASTING, falseEasting);
    double falseNorthing = _odf->value("Projection","False Northing").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvFALSENORTHING, falseNorthing);
    double centralMeridian = _odf->value("Projection","Central Meridian").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvCENTRALMERIDIAN, centralMeridian);
    double centralParllel = _odf->value("Projection","Central Parallel").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvCENTRALPARALLEL, centralParllel);
    double standardParllel = _odf->value("Projection","Standard Parallel 1").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvSTANDARDPARALLEL1, standardParllel);
    standardParllel = _odf->value("Projection","Standard Parallel 2").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvSTANDARDPARALLEL2, standardParllel);
    double lattitudeOfTrueScale = _odf->value("Projection","Latitude of True Scale").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvLATITUDEOFTRUESCALE, lattitudeOfTrueScale);
    double height = _odf->value("Projection","Height Persp. Center").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvHEIGHT, height);
    double scaleFactor = _odf->value("Projection","Scale Factor").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvSCALE, scaleFactor);
    int gzone = _odf->value("Projection","Zone").toInt(&ok);
    if ( ok)
        proj->setParameter(Projection::pvZONE, gzone);
    QString hemisphere = _odf->value("Projection","Northern Hemisphere");
    if ( hemisphere != sUNDEF && code == "utm")
        proj->setParameter(Projection::pvNORTH, hemisphere);

    return proj;
}

bool CoordinateSystemConnector::storeMetaData(IlwisObject *data, const IOOptions &options) {
    bool ok = true;
    if ((ok = Ilwis3Connector::storeMetaData(data, itCOORDSYSTEM)) == false){
        return false;
    }
    ICoordinateSystem csy(static_cast<CoordinateSystem *>(data));
    if (!csy.isValid() || !csy->isValid()){
        return ERROR1(ERR_NO_INITIALIZED_1, "CoordinateSystem");
    }
    /*
    Envelope bounds = csy->envelope();
    if(!bounds.isValid()){
        ERROR2(ERR_NO_INITIALIZED_2, "Bounds", csy->name());
        return sUNDEF;
    }

    _odf->setKeyValue("CoordSystem","CoordBounds",QString("%1 %2 %3 %4").
                      arg(bounds.min_corner().x,10,'f').
                      arg(bounds.min_corner().y,10,'f').
                      arg(bounds.max_corner().x,10,'f').
                      arg(bounds.max_corner().y,10,'f'));
    */
    if (csy->ilwisType() == itCONVENTIONALCOORDSYSTEM) {
        IConventionalCoordinateSystem cCsy = csy.as<ConventionalCoordinateSystem>();
        if ( csy->isLatLon()) {
            _odf->setKeyValue("CoordSystem","Type","LatLon");
            _odf->setKeyValue("Ilwis","Class","Coordinate System LatLon"); // overrule Ilwis3Connector::storeMetaData() because only here we can distinguish between projected and latlon
        } else if( cCsy->projection().isValid()) {
            QString projectionName = Ilwis3Connector::code2name(cCsy->projection()->code(), "projection");
            _odf->setKeyValue("CoordSystem","Type","Projection");
            _odf->setKeyValue("Ilwis","Class","Coordinate System Projection");
            _odf->setKeyValue("CoordSystem","Projection",projectionName);
            _odf->setKeyValue("CoordSystem", "UnitSize", 1);
            IProjection projection = cCsy->projection();
            if ( projection->isSet(Projection::pvAZIMCLINE))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvAZIMCLINE),projection->parameter(Projection::pvAZIMCLINE).toDouble());
            if ( projection->isSet(Projection::pvAZIMYAXIS))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvAZIMYAXIS),projection->parameter(Projection::pvAZIMYAXIS).toDouble());
            if ( projection->isSet(Projection::pvHEIGHT))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvHEIGHT),projection->parameter(Projection::pvHEIGHT).toDouble());
            if ( projection->isSet(Projection::pvSCALE))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvSCALE),projection->parameter(Projection::pvSCALE).toDouble());
            if ( projection->isSet(Projection::pvCENTRALPARALLEL))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvCENTRALPARALLEL),projection->parameter(Projection::pvCENTRALPARALLEL).toDouble());
            if ( projection->isSet(Projection::pvSTANDARDPARALLEL1))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvSTANDARDPARALLEL1),projection->parameter(Projection::pvSTANDARDPARALLEL1).toDouble());
            if ( projection->isSet(Projection::pvSTANDARDPARALLEL2))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvSTANDARDPARALLEL2),projection->parameter(Projection::pvSTANDARDPARALLEL2).toDouble());
            if ( projection->isSet(Projection::pvLATITUDEOFTRUESCALE))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvLATITUDEOFTRUESCALE),projection->parameter(Projection::pvLATITUDEOFTRUESCALE).toDouble());
            if ( projection->isSet(Projection::pvCENTRALMERIDIAN))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvCENTRALMERIDIAN),projection->parameter(Projection::pvCENTRALMERIDIAN).toDouble());
            if ( projection->isSet(Projection::pvNORIENTED))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvNORIENTED),projection->parameter(Projection::pvNORIENTED).toString());
            if ( projection->isSet(Projection::pvNORTH))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvNORTH),projection->parameter(Projection::pvNORTH).toString());
            if ( projection->isSet(Projection::pvFALSEEASTING))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvFALSEEASTING),projection->parameter(Projection::pvFALSEEASTING).toDouble());
            if ( projection->isSet(Projection::pvFALSENORTHING))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvFALSENORTHING),projection->parameter(Projection::pvFALSENORTHING).toDouble());
            if ( projection->isSet(Projection::pvZONE))
                _odf->setKeyValue("Projection",prjParam2IlwisName(Projection::pvZONE),projection->parameter(Projection::pvZONE).toInt());
        }

        const std::unique_ptr<Ilwis::GeodeticDatum> & datum = cCsy->datum();
        if (datum && datum->isValid()) {
            QString datumName = datum->code();
            if (datumName.length() > 0 && datumName != sUNDEF) { // a predefined datum in ilwis3 includes the ellipsoid definition
                datumName = Ilwis3Connector::code2name(datumName, "datum");
                if (datumName != sUNDEF) {
                    QStringList nameArea = datumName.split("|");
                    _odf->setKeyValue("CoordSystem","Datum",nameArea[0]);
                    if (nameArea.length() > 1)
                        _odf->setKeyValue("CoordSystem","Datum Area",nameArea[1]);
                    else
                        _odf->setKeyValue("CoordSystem","Datum Area","");
                } else // no datum found; defaulting to wgs84
                    _odf->setKeyValue("CoordSystem", "Datum", "WGS 1984");
            } else { // user defined
                if ( cCsy->ellipsoid().isValid()) { // write out ellipsoid for user-defined datum
                    QString ellipsoidName = cCsy->ellipsoid()->code();
                    if ( ellipsoidName != sUNDEF) {
                        ellipsoidName = Ilwis3Connector::code2name(ellipsoidName, "ellipsoid");
                        if (ellipsoidName.length() == 0 || ellipsoidName == sUNDEF)
                            ellipsoidName = "WGS 84";
                    } else {
                        ellipsoidName = "User Defined";
                        _odf->setKeyValue("Ellipsoid","a", cCsy->ellipsoid()->majorAxis() );
                        _odf->setKeyValue("Ellipsoid","1/f", 1.0 / cCsy->ellipsoid()->flattening() );
                    }
                    _odf->setKeyValue("CoordSystem","Ellipsoid",ellipsoidName);
                } else
                    _odf->setKeyValue("CoordSystem","Ellipsoid","WGS 84");
                _odf->setKeyValue("CoordSystem", "Datum", "User Defined");
                switch(datum->getTransformationMode()) {
                case GeodeticDatum::dtMolodensky:
                    _odf->setKeyValue("Datum", "dx", datum->parameter(GeodeticDatum::dmDX));
                    _odf->setKeyValue("Datum", "dy", datum->parameter(GeodeticDatum::dmDY));
                    _odf->setKeyValue("Datum", "dz", datum->parameter(GeodeticDatum::dmDZ));
                    break;
                case GeodeticDatum::dtBursaWolf:
                    _odf->setKeyValue("Datum", "dx", datum->parameter(GeodeticDatum::dmDX));
                    _odf->setKeyValue("Datum", "dy", datum->parameter(GeodeticDatum::dmDY));
                    _odf->setKeyValue("Datum", "dz", datum->parameter(GeodeticDatum::dmDZ));
                    _odf->setKeyValue("Datum", "rotX", datum->parameter(GeodeticDatum::dmRX));
                    _odf->setKeyValue("Datum", "rotY", datum->parameter(GeodeticDatum::dmRY));
                    _odf->setKeyValue("Datum", "rotZ", datum->parameter(GeodeticDatum::dmRZ));
                    _odf->setKeyValue("Datum", "dS", datum->parameter(GeodeticDatum::dmSCALE));
                    _odf->setKeyValue("Datum", "Type", "User Defined BursaWolf");
                    break;
                case GeodeticDatum::dtBadekas:
                    _odf->setKeyValue("Datum", "dx", datum->parameter(GeodeticDatum::dmDX));
                    _odf->setKeyValue("Datum", "dy", datum->parameter(GeodeticDatum::dmDY));
                    _odf->setKeyValue("Datum", "dz", datum->parameter(GeodeticDatum::dmDZ));
                    _odf->setKeyValue("Datum", "rotX", datum->parameter(GeodeticDatum::dmRX));
                    _odf->setKeyValue("Datum", "rotY", datum->parameter(GeodeticDatum::dmRY));
                    _odf->setKeyValue("Datum", "rotZ", datum->parameter(GeodeticDatum::dmRZ));
                    _odf->setKeyValue("Datum", "dS", datum->parameter(GeodeticDatum::dmSCALE));
                    _odf->setKeyValue("Datum", "X0", datum->parameter(GeodeticDatum::dmCENTERXR));
                    _odf->setKeyValue("Datum", "Y0", datum->parameter(GeodeticDatum::dmCENTERYR));
                    _odf->setKeyValue("Datum", "Z0", datum->parameter(GeodeticDatum::dmCENTERXR));
                    _odf->setKeyValue("Datum", "Type", "User Defined Badekas");
                    break;
                }
            }
        } else if ( cCsy->ellipsoid().isValid()) { // no datum known: write out ellipsoid
            QString ellipsoidName = cCsy->ellipsoid()->code();
            if ( ellipsoidName != sUNDEF) {
                ellipsoidName = Ilwis3Connector::code2name(ellipsoidName, "ellipsoid");
                if (ellipsoidName.length() == 0 || ellipsoidName == sUNDEF)
                    ellipsoidName = "WGS 84";
            } else {
                ellipsoidName = "User Defined";
                _odf->setKeyValue("Ellipsoid","a", cCsy->ellipsoid()->majorAxis() );
                _odf->setKeyValue("Ellipsoid","1/f", 1.0 / cCsy->ellipsoid()->flattening() );
            }
            _odf->setKeyValue("CoordSystem","Ellipsoid",ellipsoidName);
        } else // both datum and ellipsoid unknown: defaulting to datum wgs84
            _odf->setKeyValue("CoordSystem", "Datum", "WGS 1984");
    }

    _odf->setKeyValue("CoordSystem","Width",28);
    _odf->setKeyValue("CoordSystem","Decimals",2);
    _odf->setKeyValue("Domain", "Type", "DomainCoord");
    _odf->store("csy", sourceRef().toLocalFile());
    return true;
}

bool CoordinateSystemConnector::storeBinaryData(IlwisObject* )
{
    return true;
}

QString CoordinateSystemConnector::format() const
{
    return "coordsystem";
}
