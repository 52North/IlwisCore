#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QSettings>
#include <QUrlQuery>

#include "kernel.h"
#include "domainitem.h"
#include "raster.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "polygon.h"
#include "geometry.h"
#include "feature.h"
#include "featurecoverage.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"
#include "textdomain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "resource.h"
#include "geodeticdatum.h"
#include "internalilwisobjectfactory.h"
#include "ellipsoid.h"
#include "projection.h"
#include "projectionimplementation.h"
#include "projectionfactory.h"
#include "columndefinition.h"
#include "table.h"
#include "georefimplementation.h"
#include "georefimplementationfactory.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "ilwisobjectconnector.h"
#include "internalgridcoverageconnector.h"
#include "basetable.h"
#include "flattable.h"
#include "databasetable.h"
#include "conventionalcoordinatesystem.h"
#include "operationmetadata.h"
#include "epsg.h"

using namespace Ilwis;
using namespace Internal;

InternalIlwisObjectFactory::InternalIlwisObjectFactory() : IlwisObjectFactory("IlwisObjectFactory", "internal", "")
{
}

Ilwis::IlwisObject *InternalIlwisObjectFactory::create(const Resource& item) const
{
    if ( item.url().scheme()!="ilwis")
        return 0;


    if ( item.ilwisType() & itELLIPSOID) {
        return createEllipsoid(item);
    } else if ( item.ilwisType() & itPROJECTION) {
        return createProjection(item) ;
    }  else if ( item.ilwisType() & itDOMAIN) {
        return createDomain(item);
    } else if ( item.ilwisType() & itCOORDSYSTEM) {
        return createCsyFromCode(item);
    } else if ( item.ilwisType() & itRASTER) {
        return createRasterCoverage(item);
    } else if ( item.ilwisType() & itTABLE) {
        return createTable(item);
    } else if ( item.ilwisType() & itOPERATIONMETADATA) {
        return createOperationMetaData(item);
    } else if ( item.ilwisType() & itGEOREF) {
        return createGeoreference(item);
    } else if ( item.ilwisType() & itFEATURE) {
        return createFeatureCoverage(item);
    }
    return 0;
}

IlwisObject *InternalIlwisObjectFactory::createFeatureCoverage(const Resource& item) const{
    if ( !item.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,"resource");
        return 0;
    }
    FeatureCoverage *fcoverage = new FeatureCoverage(item);
    if (!createCoverage(item, fcoverage))
        return 0;

    return fcoverage;

}

IlwisObject *InternalIlwisObjectFactory::createOperationMetaData(const Resource& item) const{
    return new OperationMetaData(item);
}

IlwisObject *InternalIlwisObjectFactory::create(IlwisTypes type, const QString& subtype) const
{
    switch(type) {
    case itITEMDOMAIN:
    {
        QString sub = subtype.toLower();
        if (  sub == "thematic")
            return new ItemDomain<ThematicItem>();
        if ( sub == "identifier")
            return new ItemDomain<NamedIdentifier>();
        if ( sub == "indexed")
            return new ItemDomain<IndexedIdentifier>();
    }
    case itTEXTDOMAIN:
        return new TextDomain();
    case itRASTER:
        return new RasterCoverage();
    case itTABLE:
        return new FlatTable();
    case itGEOREF:
        return new GeoReference();
    case itCONVENTIONALCOORDSYSTEM:
        return new ConventionalCoordinateSystem();
    case itNUMERICDOMAIN:
        return new NumericDomain();
    case itPROJECTION:
        return new Projection();
    case itELLIPSOID:
        return new Ellipsoid();
    case itOPERATIONMETADATA:
        return new OperationMetaData();
    }
    if ( type & itFEATURE)
        return new FeatureCoverage();
    return 0;
}

bool InternalIlwisObjectFactory::canUse(const Resource& item) const
{
    if ( item.url().scheme()!="ilwis")
        return false;

    if ( item.ilwisType() & itELLIPSOID) {
        return true;
    } else if ( item.ilwisType() & itPROJECTION) {
        return true;
    } else if ( item.ilwisType() & itGEODETICDATUM) {
        return true;
    } else if ( item.ilwisType() & itDOMAIN) {
        return true;
    } else if ( item.ilwisType() & itCOORDSYSTEM) {
        return true;
    } else if ( item.ilwisType() & itRASTER) {
        return true;
    } else if ( item.ilwisType() & itTABLE) {
        return true;
    }else if ( item.ilwisType() & itOPERATIONMETADATA) {
        return true;
    } else if ( item.ilwisType() & itGEOREF) {
        return true;
    } else if ( item.ilwisType() & itFEATURE) {
        return true;
    }

    return false;
}



bool InternalIlwisObjectFactory::createCoverage(const Resource& item, Coverage *coverage) const {

    if (!coverage->prepare())
        return false;

    coverage->setName(QString("%1%2").arg(ANONYMOUS_PREFIX).arg(coverage->id()));

    ICoordinateSystem csy;
    if (QString(item["coordinatesystem"].typeName()) == "Ilwis::ICoordinateSystem")
        csy = item["coordinatesystem"].value<Ilwis::ICoordinateSystem>();
    else if( QString(item["coordinatesystem"].typeName()) == "QString" &&
             item["coordinatesystem"].toString() != sUNDEF  ) {
        Resource resource = property2Resource(item["coordinatesystem"], itCOORDSYSTEM);
        if ( resource.isValid()) {
            if (!csy.prepare(resource))
                return false;
        }
    }
    if ( csy.isValid()){
        coverage->setCoordinateSystem(csy);
    }

    Box2D<double> bounds;
    if ( QString(item["envelope"].typeName()) == "Ilwis::Box2D<double>") {
        bounds = item["envelope"].value<Ilwis::Box2D<double>>();
    }else if (QString(item["envelope"].typeName()) == "QString" &&
              item["envelope"].toString() != sUNDEF) {
        bounds = Box2D<double>(item["envelope"].toString());
    }
    if ( bounds.isValid()) {
        coverage->envelope(bounds);
    }
    IDomain dom;
    QString gg = QString(item["domain"].typeName());
    if (QString(item["domain"].typeName()) == "Ilwis::IDomain")
        dom = item["domain"].value<Ilwis::IDomain>();
    else if( QString(item["domain"].typeName()) == "QString" &&
             item["domain"].toString() != sUNDEF  ) {
        Resource resource = property2Resource(item["domain"], itDOMAIN);
        if ( resource.isValid()) {
            if (!dom.prepare(resource))
                return false;
        }
    }
    if ( dom.isValid()){
        coverage->datadef().domain(dom);
    }
    return true;
}

IlwisObject *InternalIlwisObjectFactory::createRasterCoverage(const Resource& item) const {
    if ( !item.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,"resource");
        return 0;
    }
    RasterCoverage *gcoverage = new RasterCoverage(item);
    if (!createCoverage(item, gcoverage))
        return 0;

    Size sz;
    if ( QString(item["size"].typeName()) == "Ilwis::Size"){
        sz = item["size"].value<Size>();
    } else if (QString(item["size"].typeName()) == "QSize") {
        sz = item["size"].toSize();
    }

    IGeoReference grf;
    if (QString(item["georeference"].typeName()) == "Ilwis::IGeoReference")
        grf = item["georeference"].value<Ilwis::IGeoReference>();
    else if( QString(item["georeference"].typeName()) == "QString"  &&
             item["georeference"].toString() != sUNDEF) {
        Resource resource = property2Resource(item["georeference"], itGEOREF);
        if ( resource.isValid()) {
            if (!grf.prepare(resource))
                return 0;
        }
    } else{
        Box2D<double> bounds = gcoverage->envelope();
        if ( bounds.isValid() && !bounds.isNull()){
//            GeoReference *cgeoref = new GeoReference();
//            GeoRefImplementationFactory *grfFac = kernel()->factory<GeoRefImplementationFactory>("georefimplementationfactory");
//            GeoRefImplementation *impl = grfFac->create("corners");
//            if ( !impl) {
//                ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"corners georef",item.name());
//                return 0;
//            }
            grf = GeoReference::create("corners");
            grf->setName("subset_" + gcoverage->name());
            grf->coordinateSystem(gcoverage->coordinateSystem());
            grf->impl<CornersGeoReference>()->setEnvelope(bounds);
            grf->size(sz);
            if (!grf->compute()){
                ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1, "Georeference");
                return 0;
            }

        }

    }
    if ( grf.isValid())
        gcoverage->georeference(grf);
    if ( sz.isValid())
        gcoverage->size(sz);

    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!factory) {
        ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1, "ilwis::ConnectorFactory");
        return 0;
    }
    InternalRasterCoverageConnector *connector = factory->createFromResource<InternalRasterCoverageConnector>(item, "internal");
    if ( !connector) {
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "connector", item.name());
        return 0;
    }
    gcoverage->setConnector(connector);
    connector->loadMetaData(gcoverage);

    return gcoverage;
}

Resource InternalIlwisObjectFactory::property2Resource(const QVariant& property, IlwisTypes type) const{
    if ( !property.isValid() || property.isNull() )
        return Resource();
    return mastercatalog()->name2Resource(property.toString(), type);
}

IlwisObject *InternalIlwisObjectFactory::createDomain(const Resource& resource) const{
    QString code = resource.code();
    if ( code != sUNDEF) {
        if ( code == "text")
            return new TextDomain(resource);
        QSqlQuery db(kernel()->database());
        QString query = QString("Select linkedtable from codes where code = '%1'").arg(code);
        if (db.exec(query)) {
            if ( db.next()){
                QString table = db.value(0).toString();
                if ( table == "numericdomain"){
                    query = QString("Select * from numericdomain where code='%1'").arg(code);
                    if (db.exec(query)) {
                        if ( db.next()){
                            QSqlRecord rec = db.record();
                            NumericDomain *dv = new NumericDomain(resource);

                            dv->fromInternal(rec);
                            double vmin = rec.field("minv").value().toDouble();
                            double vmax = rec.field("maxv").value().toDouble();
                            double step = rec.field("resolution").value().toDouble();
                            int range_strict = rec.field("range_strict").value().toInt();
                            // TODO, implement unit stuff
                            //QString unit = rec.field("unit").value().toString();
                            if ( fmod(step,1.0) == 0 && step != 0)
                                dv->setRange(new NumericRange(vmin, vmax,1));
                            else
                                dv->setRange(new NumericRange(vmin, vmax));

                            dv->setStrict(range_strict ? true : false);
                            QString parent = rec.field("parent").value().toString();
                            if ( parent != "" && parent !=  code) { // no parenting to itself
                                IDomain dom;
                                dom.prepare(parent);
                                if ( dom.isValid()) {
                                    dv->setParent(dom);
                                }
                            }
                            return dv;
                        }else {
                            kernel()->issues()->log(TR(ERR_FIND_SYSTEM_OBJECT_1).arg(code));
                        }
                    }

                } else if (table == "thematicdomain"){
                    //TODO internal thematic domains
                }

            }
        }else {
            kernel()->issues()->log(TR(ERR_FIND_SYSTEM_OBJECT_1).arg(code));
        }
    }else {
        if ( hasType(resource.ilwisType(), itITEMDOMAIN )){
            if ( hasType(resource.ilwisType(), itNAMEDITEM)) {
                Resource res = resource;
                res.setIlwisType(itITEMDOMAIN);
                return new ItemDomain<NamedIdentifier>(res);
            }
        }
    }

    return 0;
}

IlwisObject *InternalIlwisObjectFactory::createCsyFromCode(const Resource& item) const {
    QString code = item.code();
    QString projParms = code;
    if ( code.left(6) != "proj4:"){
        QString query = QString("select * from projectedcsy where code='%1'").arg(code);
        QSqlQuery db(kernel()->database());
        if ( db.exec(query)) {
            if (db.next()) {
                QSqlRecord rec = db.record();
                projParms = rec.value("proj_params").toString();
            } else {
                kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("coordinatesystem", item.name()));
                return 0;
            }
        }
    } else {
        projParms = code.mid(6);
    }
    ConventionalCoordinateSystem *csy = new ConventionalCoordinateSystem(item);
    csy->setName(item.name());
    csy->setCode(item.code());
    csy->prepare("proj4=" + projParms);
    return csy;

}

IlwisObject *InternalIlwisObjectFactory::createProjection(const Resource& item) const {
    QString query;
    QString code = item.code();
    if ( code != "") {
        QSqlQuery db(kernel()->database());
        query = QString("Select * from projection where code = '%1'").arg(code);
        if ( db.exec(query)) {
            if (db.next()) {
                QSqlRecord rec = db.record();
                //if ( code == "longlat") // special case
                //    return new NullProjection(item);
                const ProjectionFactory *factory =  kernel()->factory<ProjectionFactory>("ProjectionFactory",item);
                if ( factory) {
                    ProjectionImplementation *projimpl = static_cast<ProjectionImplementation *>(factory->create(item));
                    if (!projimpl) {
                        kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("projection", item.name()));
                        return 0;
                    }
                    Projection *proj = new Projection();
                    proj->setImplementation(projimpl);
                    proj->fromInternal(rec);
                    proj->setAuthority(rec.field("authority").value().toString());
                    proj->setWkt(rec.field("wkt").value().toString());
                    return proj;
                } else {
                    kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("ProjectionFactory",item.name()));
                }
            } else {
                kernel()->issues()->log(TR(ERR_FIND_SYSTEM_OBJECT_1).arg(code));
            }
        } else{
            kernel()->issues()->logSql(db.lastError());
        }
    } else {
        kernel()->issues()->log(TR(ERR_MISSING_CODE_FOR_SYSTEM_OBJECT));
    }
    return 0;
}

GeodeticDatum *InternalIlwisObjectFactory::createDatum(const Resource& item) const {
    QString query;
    if ( item.code() != sUNDEF) {
        QString code = item.code();
        if ( code != "") {
            query = QString("Select * from datum where code = '%1'").arg(code);
        }
    }
    if ( item["area"] != sUNDEF) {
        QString name = item.name();
        QString area = item["area"].toString();
        query = QString("Select * from datum where name='%1' and area='%1'").arg(name, area);
    }

    if ( query == "")
        return 0;

    QSqlQuery db(kernel()->database());
    if (db.exec(query) && db.next()) {
        GeodeticDatum *datum = new GeodeticDatum();
        QSqlRecord rec = db.record();
        datum->setName(rec.field("name").value().toString());
        datum->setDescription(rec.field("description").value().toString());
        datum->setAuthority(rec.field("authority").value().toString());
        datum->setArea(rec.field("area").value().toString());
        datum->setCode(rec.field("code").value().toString());
        datum->set3TransformationParameters(rec.field("dx").value().toDouble(),
                                            rec.field("dy").value().toDouble(),
                                            rec.field("dz").value().toDouble());

        return datum;
    }

    return 0;
}

IlwisObject *InternalIlwisObjectFactory::createGeoreference(const Resource& item) const {
    GeoReference *cgrf = GeoReference::create("corners");
    cgrf->setName( item["name"].toString());
    cgrf->setCreateTime(Time::now());
    cgrf->setModifiedTime(Time::now());
    cgrf->coordinateSystem(item["coordinatesystem"].value<ICoordinateSystem>());
    cgrf->impl<CornersGeoReference>()->setEnvelope(item["envelope"].value<Box2D<double>>());
//    Size sz = item["size"].value<Size>();
    cgrf->size(item["size"].value<Size>());
    cgrf->centerOfPixel(item["centerofpixel"].toBool());

    return cgrf;
}

IlwisObject *InternalIlwisObjectFactory::createTable(const Resource& item) const {

    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    ConnectorInterface *connector = factory->createFromResource<>(item, "internal");

   if(!connector) {
       kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",item.name()));
       return 0;
   }
    Table *tbl;
    if ( item.ilwisType() == itFLATTABLE)
        tbl = new FlatTable(item);
    else
        tbl = new DatabaseTable(item);
    tbl->setConnector(connector);
    return tbl;

}
IlwisObject *InternalIlwisObjectFactory::createEllipsoid(const Resource& item) const {
    QString query    ;
    QString code = item.code();

    if ( code != sUNDEF) {
        query = QString("Select * from ellipsoid where code = '%1'").arg(code);

    }else {
        QUrlQuery queryItem(item.url());
        if ( queryItem.hasQueryItem("wkt")) {

            QString wkt = queryItem.queryItemValue("wkt");
            if ( wkt != "") {
                query = QString("Select * from ellipsoid where wkt = '%1'").arg(wkt);
            }

        }
    }
    return createEllipsoidFromQuery(query, item);
}

IlwisObject *InternalIlwisObjectFactory::createEllipsoidFromQuery(const QString &query, const Resource& resource) const {
    if ( query == "") {
        kernel()->issues()->log(TR("No valid ellipsoid query could be made"));
        return 0;
    }

    QSqlQuery db(kernel()->database());
    if (db.exec(query) && db.next()) {
        Ellipsoid *ellipsoid = new Ellipsoid(resource);
        ellipsoid->fromInternal(db.record());
        double ma = db.record().field("majoraxis").value().toDouble();
        double rf = db.record().field("invflattening").value().toDouble();
        ellipsoid->setEllipsoid(ma,rf);
        ellipsoid->setAuthority(db.record().field("authority").value().toString());
        return ellipsoid;
    }
    return 0;
}

