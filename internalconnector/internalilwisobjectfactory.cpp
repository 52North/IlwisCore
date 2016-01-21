#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QSettings>
#include <QUrlQuery>
#include <QColor>

#include "raster.h"
#include "domainitem.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "juliantime.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"
#include "textdomain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "identifierrange.h"
#include "colorrange.h"
#include "colordomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "interval.h"
#include "coloritem.h"
#include "resource.h"
#include "intervalrange.h"
#include "geodeticdatum.h"
#include "internalilwisobjectfactory.h"
#include "ellipsoid.h"
#include "projection.h"
#include "projectionimplementation.h"
#include "projectionfactory.h"
#include "georefimplementation.h"
#include "georefimplementationfactory.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "ilwisobjectconnector.h"
#include "internalrastercoverageconnector.h"
#include "basetable.h"
#include "flattable.h"
#include "colorlookup.h"
#include "continuouscolorlookup.h"
#include "palettecolorlookup.h"
#include "representation.h"
#include "boundsonlycoordinatesystem.h"
#include "conventionalcoordinatesystem.h"
#include "operationmetadata.h"
#include "epsg.h"
#include "catalog.h"
#include "workflow.h"


using namespace Ilwis;
using namespace Internal;

InternalIlwisObjectFactory::InternalIlwisObjectFactory() : IlwisObjectFactory("IlwisObjectFactory", "internal", "")
{
}

Ilwis::IlwisObject *InternalIlwisObjectFactory::create(const Resource& resource, const IOOptions &options) const
{
    if ( resource.url().scheme()!="ilwis")
        return 0;

    if ( resource.url(true).scheme() == "file") { // we might have a streamed object
        QString filename = resource.url(true).toLocalFile();
        QFileInfo inf(filename);
        if ( inf.exists()) // internal objects never exist on disk,
            return 0;
    }


    if ( resource.ilwisType() & itELLIPSOID) {
        return createEllipsoid(resource,options);
    } else if ( resource.ilwisType() & itPROJECTION) {
        return createProjection(resource,options) ;
    }  else if ( resource.ilwisType() & itDOMAIN) {
        return createDomain(resource,options);
    } else if ( resource.ilwisType() & itCOORDSYSTEM) {
        if ( resource.code() != sUNDEF)
            return createCsyFromCode(resource,options);
        return createCsy(resource,options);
    } else if ( resource.ilwisType() & itRASTER) {
        return createRasterCoverage(resource,options);
    } else if ( resource.ilwisType() & itTABLE) {
        return createTable(resource,options);
    } else if ( resource.ilwisType() & itWORKFLOW) {
        return createWorkflow(resource, options);
    } else if ( resource.ilwisType() & itSINGLEOPERATION) {
        return createOperationMetaData(resource, options);
    } else if ( resource.ilwisType() & itGEOREF) {
        return createGeoreference(resource,options);
    } else if ( resource.ilwisType() & itFEATURE) {
        return createFeatureCoverage(resource,options);
    } else if ( resource.ilwisType() & itCATALOG) {
        return createCatalog(resource,options);
    } else if ( resource.ilwisType() & itREPRESENTATION) {
        return createRepresentation(resource,options);
    }
    return 0;
}

IlwisObject *InternalIlwisObjectFactory::createRepresentation(const Resource& resource, const IOOptions &options) const{
    QString code = resource.code();
    Representation *rpr = new Representation(resource);
    if ( code != sUNDEF) {

        InternalDatabaseConnection db;
        QString query = QString("Select linkedtable from codes where code = '%1'").arg(code);
        if (db.exec(query)) {
            if ( db.next()){
                QString table = db.value(0).toString();
                if ( table == "representation"){
                    query = QString("Select * from representation where code='%1'").arg(code);
                    if (db.exec(query)) {
                        if ( db.next()){
                            QSqlRecord rec = db.record();
                            rpr->fromInternal(rec);
                            QString relateddomain = rec.field("relateddomain").value().toString();
                            QString rprType = rec.field("representationtype").value().toString();
                            QString  definition = rec.field("definition").value().toString();
                            if ( rprType == "continuouscolor"){
                                rpr->colors(new ContinuousColorLookup(definition));
                            }else if ( rprType == "palettecolor"){
                                rpr->colors(new PaletteColorLookUp(definition));
                            }
                            if ( relateddomain == "value"){
                                rpr->domain(IDomain("value"));
                            }
                            rpr->readOnly(true);
                        }else
                            return 0;
                    }else
                        return 0;
                }else
                    return 0;
            }else
                return 0;
        }else
            return 0;
    }

    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    ConnectorInterface *connector = factory->createFromResource<>(resource, "internal");
    rpr->setConnector(connector,IlwisObject::cmINPUT, options);
    return rpr;
}

IlwisObject *InternalIlwisObjectFactory::createCatalog(const Resource& resource, const IOOptions &options) const{
    if ( hasType(resource.ilwisType(), itCATALOG)){
        Catalog *cat = new Catalog(resource);
        const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
        if (!factory) {
            ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1, "ilwis::ConnectorFactory");
            return 0;
        }
        ConnectorInterface *connector = factory->createFromResource<>(resource, "ilwis");
        if ( !connector) {
            ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "connector", resource.name());
            return 0;
        }
        cat->setConnector(connector, IlwisObject::cmINPUT, options);

        return cat;

    }
    return nullptr;
}

IlwisObject *InternalIlwisObjectFactory::createFeatureCoverage(const Resource& resource, const IOOptions &options) const{
    if ( !resource.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,"resource");
        return 0;
    }
    FeatureCoverage *fcoverage = new FeatureCoverage(resource);
    if (!createCoverage(resource, fcoverage, options))
        return 0;

    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!factory) {
        ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1, "ilwis::ConnectorFactory");
        return 0;
    }
    ConnectorInterface *connector = factory->createFromResource<>(resource, "internal");
    if ( !connector) {
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "connector", resource.name());
        return 0;
    }
    fcoverage->setConnector(connector, IlwisObject::cmINPUT, options);

    return fcoverage;

}

IlwisObject *InternalIlwisObjectFactory::createOperationMetaData(const Resource& resource, const IOOptions &options) const{
    //return new OperationMetaData(resource);
    if (!hasType(resource.ilwisType(), itSINGLEOPERATION)){
        return nullptr;
    }
    OperationMetaData *metadata = new OperationMetaData(resource);

    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!factory) {
        ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1, "ilwis::ConnectorFactory");
        return 0;
    }
    ConnectorInterface *connector = factory->createFromResource<>(resource, "internal");
    if ( !connector) {
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "connector", resource.name());
        return 0;
    }
    metadata->setConnector(connector, IlwisObject::cmINPUT, options);
    metadata->init();

    return metadata;
}

IlwisObject *InternalIlwisObjectFactory::createWorkflow(const Resource& resource, const IOOptions &options) const {
    if (!hasType(resource.ilwisType(), itWORKFLOW)){
        return nullptr;
    }
    Workflow *workflow = new Workflow(resource);

    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!factory) {
        ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1, "ilwis::ConnectorFactory");
        return 0;
    }
    ConnectorInterface *connector = factory->createFromResource<>(resource, "internal");
    if ( !connector) {
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "connector", resource.name());
        return 0;
    }
    workflow->setConnector(connector, IlwisObject::cmINPUT, options);

    return workflow;
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
        if ( sub == "palette")
            return new ItemDomain<ColorItem>();
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
    case itBOUNDSONLYCSY:
        return new BoundsOnlyCoordinateSystem();
    case itNUMERICDOMAIN:
        return new NumericDomain();
    case itCOLORDOMAIN:
        return new ColorDomain();
    case itPROJECTION:
        return new Projection();
    case itELLIPSOID:
        return new Ellipsoid();
    case itSINGLEOPERATION:
        return new OperationMetaData();
    case itWORKFLOW:
        return new Workflow();
    case itREPRESENTATION:
        return new Representation();
    }
    if ( type & itFEATURE)
        return new FeatureCoverage();
    return 0;
}

bool InternalIlwisObjectFactory::canUse(const Resource& resource) const
{
    if ( resource.url().scheme()!="ilwis"){
        return false;
    }
    if ( resource.url(true).scheme() == "file") { // we might have a streamed object
        QString filename = resource.url(true).toLocalFile();
        QFileInfo inf(filename);
        if ( inf.exists()) // internal objects never exist on disk,
            return false;
    }

    if ( resource.ilwisType() & itELLIPSOID) {
        return true;
    } else if ( resource.ilwisType() & itPROJECTION) {
        return true;
    } else if ( resource.ilwisType() & itGEODETICDATUM) {
        return true;
    } else if ( resource.ilwisType() & itDOMAIN) {
        return true;
    } else if ( resource.ilwisType() & itCOORDSYSTEM) {
        return true;
    } else if ( resource.ilwisType() & itRASTER) {
        return true;
    } else if ( resource.ilwisType() & itTABLE) {
        return true;
    }else if ( resource.ilwisType() & itOPERATIONMETADATA) {
        return true;
    } else if ( resource.ilwisType() & itGEOREF) {
        return true;
    } else if ( resource.ilwisType() & itFEATURE) {
        return true;
    } else if ( resource.ilwisType() & itCATALOG) {
        return true;
    }  else if ( resource.ilwisType() & itREPRESENTATION) {
        return true;
    }

    return false;
}



bool InternalIlwisObjectFactory::createCoverage(const Resource& resource, Coverage *coverage, const IOOptions &options) const {

    if (!coverage->prepare())
        return false;

    //coverage->setName(QString("%1%2").arg(ANONYMOUS_PREFIX).arg(coverage->id()));

    ICoordinateSystem csy;
    QString typnm = resource["coordinatesystem"].typeName();
    if (typnm == "Ilwis::ICoordinateSystem")
        csy = resource["coordinatesystem"].value<Ilwis::ICoordinateSystem>();
    else if( typnm == "QString" &&
             resource["coordinatesystem"].toString() != sUNDEF  ) {
        Resource newresource = resource.property2Resource("coordinatesystem", itCOORDSYSTEM);
        if ( newresource.isValid()) {
            if (!csy.prepare(newresource,options))
                return false;
        }
    } else if ( typnm == "qulonglong"){
        if(!csy.prepare(resource["coordinatesystem"].value<quint64>()))
            return 0;
    }
    if ( csy.isValid()){
        coverage->coordinateSystem(csy);
    }

    Envelope bounds;
    QString envType = resource["envelope"].typeName();
    if ( envType == "Ilwis::Box<double>" || envType == "Ilwis::Envelope") {
        bounds = resource["envelope"].value<Envelope>();
    }else if (QString(resource["envelope"].typeName()) == "QString" &&
              resource["envelope"].toString() != sUNDEF) {
        bounds = Envelope(resource["envelope"].toString());
    }
    if ( bounds.isValid()) {
        coverage->envelope(bounds);
    }
    if ( resource.ilwisType() == itRASTER) {
        IDomain dom;
        QString tpname = resource["domain"].typeName();
        if (tpname == "Ilwis::IDomain")
            dom = resource["domain"].value<Ilwis::IDomain>();
        else if( tpname == "QString" &&
                 resource["domain"].toString() != sUNDEF  ) {
            Resource newresource = resource.property2Resource("domain", itDOMAIN);
            if ( newresource.isValid()) {
                if (!dom.prepare(newresource, options))
                    return false;
            }
        } else if ( tpname == "qulonglong"){
            if(!dom.prepare(resource["domain"].value<quint64>()))
                return 0;
        }

        if ( dom.isValid()){
            RasterCoverage *raster = static_cast<RasterCoverage *>(coverage);
            raster->datadefRef().domain(dom);
        }
    }
    return true;
}

IlwisObject *InternalIlwisObjectFactory::createRasterCoverage(const Resource& resource, const IOOptions &options) const {
    if ( !resource.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,"resource");
        return 0;
    }
    RasterCoverage *gcoverage = new RasterCoverage(resource);
    if (!createCoverage(resource, gcoverage, options))
        return 0;

    Size<> sz;
    QString typenm = resource["size"].typeName();
    if ( typenm == "Ilwis::Size<quint32>"){
        sz = resource["size"].value<Size<>>();
    } else if (typenm == "QSize") {
        sz = resource["size"].toSize();
    } else if (typenm == "QString") {
        QStringList parts = resource["size"].toString().split(" ");
        if ( parts.size() >= 2)
            sz = Size<>(parts[0].toInt(), parts[1].toInt(), 1);
        if ( parts.size() == 3)
            sz.zsize(parts[2].toInt());
    }

    gcoverage->gridRef()->prepare(gcoverage, sz);

    IGeoReference grf;
    QString tpnam = resource["georeference"].typeName();
    if (tpnam == "Ilwis::IGeoReference")
        grf = resource["georeference"].value<Ilwis::IGeoReference>();
    else if( tpnam == "QString"  && resource["georeference"].toString() != sUNDEF) {
        Resource newresource = resource.property2Resource("georeference", itGEOREF);
        if ( newresource.isValid()) {
            if (!grf.prepare(newresource))
                return 0;
        }
    } else if ( tpnam == "qulonglong"){
        if(!grf.prepare(resource["georeference"].value<quint64>()))
                return 0;

    } else{
        Envelope bounds = gcoverage->envelope();
        if ( bounds.isValid() && !bounds.isNull()){
            grf = new GeoReference();
            grf->create("corners");
            grf->name("subset_" + gcoverage->name());
            grf->coordinateSystem(gcoverage->coordinateSystem());
            grf->envelope(bounds);
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
    InternalRasterCoverageConnector *connector = factory->createFromResource<InternalRasterCoverageConnector>(resource, "internal");
    if ( !connector) {
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "connector", resource.name());
        return 0;
    }
    gcoverage->setConnector(connector,IlwisObject::cmINPUT,options);
    //connector->loadMetaData(gcoverage, IOOptions());

    return gcoverage;
}
template<class DomainItemType, class RangeType> Domain* createItemDomain2(const QSqlRecord& rec, const Resource& resource){
    ItemDomain<DomainItemType> *itemdom = new ItemDomain<DomainItemType>(resource);
    itemdom->fromInternal(rec);
    itemdom->name(resource.name());
    RangeType range ;
    InternalDatabaseConnection itemstable;
    QString query = "Select * from domainitems where code='" + resource.code() + "'";
    if (itemstable.exec(query) ) {
        while ( itemstable.next()){
            QSqlRecord rec = itemstable.record();
            range << rec.value("itemname").toString() + "|" + rec.value("itemcode").toString() + "|" + rec.value("itemdescription").toString();
        }
        itemdom->range(range.clone());
    }
    return itemdom;
}

Domain* InternalIlwisObjectFactory::createItemDomain(Ilwis::InternalDatabaseConnection &db, const IOOptions &options, const Resource& resource) const{
    QString query = QString("Select * from itemdomain where code='%1'").arg(resource.code());
    if (db.exec(query) && db.next()) {
        QSqlRecord rec = db.record();
        QString dt = rec.value("domaintype").toString();
        if (  dt == "thematic"){
            return createItemDomain2<ThematicItem, ThematicRange>(rec,resource);
        }
    }
    return 0;
}

NumericDomain* InternalIlwisObjectFactory::createNumericDomain(const QString& code, InternalDatabaseConnection& db, const IOOptions &options, const Resource& resource) const
{
    QString query = QString("Select * from numericdomain where code='%1'").arg(code);
    if (db.exec(query)) {
        if ( db.next()){
            QSqlRecord rec = db.record();
            NumericDomain *dv = new NumericDomain(resource);

            dv->fromInternal(rec);
            double vmin = rec.field("minv").value().toDouble();
            double vmax = rec.field("maxv").value().toDouble();
            double step = rec.field("resolution").value().toDouble();
            int range_strict = rec.field("range_strict").value().toInt();
            // TODO:, implement unit stuff
            QString unit = rec.field("unit").value().toString();
            if (unit == "Days"){
                if ( fmod(step,1.0) == 0 && step != 0)
                    dv->range(new TimeInterval(Time(vmin), Time(vmax)));
                else
                    dv->range(new TimeInterval(Time(vmin), Time(vmax),Duration(QString("%1D").arg(step))));
            }else {
                if ( fmod(step,1.0) == 0 && step != 0)
                    dv->range(new NumericRange(vmin, vmax,1));
                else
                    dv->range(new NumericRange(vmin, vmax));
            }

            dv->setStrict(range_strict ? true : false);
            QString parent = rec.field("parent").value().toString();
            if ( parent != "" && parent !=  code) { // no parenting to itself
                IDomain dom;
                dom.prepare(parent, options);
                if ( dom.isValid()) {
                    dv->setParent(dom);
                }
            }
            return dv;

        }else {
            kernel()->issues()->log(TR(ERR_FIND_SYSTEM_OBJECT_1).arg(code));
        }
    }
    return 0;
}


IlwisObject *InternalIlwisObjectFactory::createDomain(const Resource& resource, const IOOptions &options) const{
    if ( resource.ilwisType() == itTEXTDOMAIN || resource.code() == "text")
        return new TextDomain(resource);

    if ( resource.ilwisType() == itCOLORDOMAIN || resource.code() == "color") {
        ColorDomain *dm = new ColorDomain(resource);
        ContinuousColorRange *rng = new ContinuousColorRange(QColor("#000000"), QColor("#FFFFFF"));
        dm->range(rng);
        return dm;
    }else if ( resource.ilwisType() == itCOLORDOMAIN || resource.code() == "colorpalette") {
        ColorDomain *dm = new ColorDomain(resource);
        Range *rng = new ColorPalette();
        dm->range(rng);
        return dm;
    }
    QString code = resource.code();
    Domain *newdomain = 0;
    bool readonlyState = false;
    if ( code != sUNDEF) {

        InternalDatabaseConnection db;
        QString query = QString("Select linkedtable from codes where code = '%1'").arg(code);
        if (db.exec(query)) {
            if ( db.next()){
                QString table = db.value(0).toString();
                if ( table == "numericdomain"){
                    newdomain = createNumericDomain(code, db, options, resource);

                }else if ( table == "itemdomain"){
                    newdomain = createItemDomain(db, options, resource);
                }
                readonlyState = true;
            }
        }else {
            kernel()->issues()->log(TR(ERR_FIND_SYSTEM_OBJECT_1).arg(code));
        }
    }else {
        if ( hasType(resource.ilwisType(), itITEMDOMAIN )){
            if ( hasType(resource.extendedType(), itNAMEDITEM)) {
                Resource res = resource;
                res.setIlwisType(itITEMDOMAIN);
                newdomain = new ItemDomain<NamedIdentifier>(res);
            }
            else if ( hasType(resource.extendedType(), itINDEXEDITEM)) {
                Resource res = resource;
                res.setIlwisType(itITEMDOMAIN);
                newdomain = new ItemDomain<IndexedIdentifier>(res);
            }
            else if ( hasType(resource.extendedType(), itTHEMATICITEM)) {
                Resource res = resource;
                res.setIlwisType(itITEMDOMAIN);
                newdomain = new ItemDomain<ThematicItem>(res);
            }
            else if ( hasType(resource.extendedType(), itNUMERICITEM)) {
                Resource res = resource;
                res.setIlwisType(itITEMDOMAIN);
                newdomain = new ItemDomain<Interval>(res);
            }
            else if ( hasType(resource.extendedType(), itPALETTECOLOR)) {
                Resource res = resource;
                res.setIlwisType(itITEMDOMAIN);
                newdomain = new ItemDomain<ColorItem>(res);
            }
        } if ( hasType(resource.ilwisType(), itNUMERICDOMAIN)){
            newdomain = new NumericDomain(resource);
        }
    }
    if ( newdomain){
        const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
        ConnectorInterface *connector = factory->createFromResource<>(resource, "internal");
        newdomain->setConnector(connector,IlwisObject::cmINPUT, options);
        newdomain->readOnly(readonlyState);
    }
    return newdomain;
}

IlwisObject *InternalIlwisObjectFactory::createCsy(const Resource& resource, const IOOptions &options) const {
    CoordinateSystem *csy = 0;
    if ( resource.ilwisType() == itBOUNDSONLYCSY){ // by default to csy unknow but this may be changed later
        csy = new BoundsOnlyCoordinateSystem(resource);
        csy->name("unknown");
        csy->code("unknown");
        csy->setDescription(TR("Unknown coordinate system"));
    }else if ( resource.ilwisType() == itCONVENTIONALCOORDSYSTEM){
        csy = new ConventionalCoordinateSystem(resource);
        csy->name(resource.name());
        csy->code(resource.code());
    }

    if ( csy){
        const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
        ConnectorInterface *connector = factory->createFromResource<>(resource, "internal");
        csy->setConnector(connector, IlwisObject::cmINPUT, options);
    }
    return csy;
}

IlwisObject *InternalIlwisObjectFactory::createCsyFromCode(const Resource& resource, const IOOptions &options) const {
    QString code = resource.code();
    bool isUnknown = code == "unknown" || code == "csy:unknown";
    QString projParms = code;
    if ( code.left(6) == "proj4:"){
        projParms = code.mid(6);
    }else if(!isUnknown){
        QString query = QString("select * from projectedcsy where code='%1'").arg(code);
        InternalDatabaseConnection db;
        if ( db.exec(query)) {
            if (db.next()) {
                QSqlRecord rec = db.record();
                projParms = rec.value("proj_params").toString();
            } else {
                kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("coordinatesystem", resource.name()));
                return 0;
            }
        }
    }
    CoordinateSystem *csy = 0;
    if ( isUnknown){
        csy = new BoundsOnlyCoordinateSystem(resource);
        csy->name("unknown");
        csy->code("unknown");
        csy->setDescription(TR("Unknown coordinate system"));
    }else {
        csy = new ConventionalCoordinateSystem(resource);
        csy->name(resource.name());
        csy->code(resource.code());
        csy->prepare("proj4=" + projParms);
    }

    if ( csy){
        const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
        ConnectorInterface *connector = factory->createFromResource<>(resource, "internal");
        csy->setConnector(connector, IlwisObject::cmINPUT, options);
    }
    return csy;

}

IlwisObject *InternalIlwisObjectFactory::createProjection(const Resource& resource, const IOOptions &options) const {
    QString query;
    QString code = resource.code();

    if ( code == sUNDEF) // meant for new projections which will be initialized later (e.g by the streaming connector)
        return new Projection();

    if ( code != "") {
        InternalDatabaseConnection db;
        query = QString("Select * from projection where code = '%1'").arg(code);
        if ( db.exec(query)) {
            if (db.next()) {
                QSqlRecord rec = db.record();

                const ProjectionFactory *factory =  kernel()->factory<ProjectionFactory>("ProjectionFactory",resource);
                if ( factory) {
                    ProjectionImplementation *projimpl = 0;
                    if ( options.contains("proj4"))
                        projimpl = static_cast<ProjectionImplementation *>(factory->create(options["proj4"].toString()));
                    else
                        projimpl = static_cast<ProjectionImplementation *>(factory->create(resource));
                    if (!projimpl) {
                        kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("projection", resource.name()));
                        return 0;
                    }
                    Projection *proj = new Projection();
                    proj->setImplementation(projimpl);
                    proj->fromInternal(rec);
                    proj->setAuthority(rec.field("authority").value().toString());
                    proj->setWkt(rec.field("wkt").value().toString());
                    return proj;
                } else {
                    kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("ProjectionFactory",resource.name()));
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

GeodeticDatum *InternalIlwisObjectFactory::createDatum(const Resource& resource, const IOOptions &options) const {
    QString query;
    if ( resource.code() != sUNDEF) {
        QString code = resource.code();
        if ( code != "") {
            query = QString("Select * from datum where code = '%1'").arg(code);
        }
    }
    if ( resource["area"] != sUNDEF) {
        QString name = resource.name();
        QString area = resource["area"].toString();
        query = QString("Select * from datum where name='%1' and area='%1'").arg(name, area);
    }

    if ( query == "")
        return 0;

    InternalDatabaseConnection db;
    if (db.exec(query) && db.next()) {
        GeodeticDatum *datum = new GeodeticDatum();
        QSqlRecord rec = db.record();
        datum->name(rec.field("name").value().toString());
        datum->setDescription(rec.field("description").value().toString());
        datum->setAuthority(rec.field("authority").value().toString());
        datum->setArea(rec.field("area").value().toString());
        datum->code(rec.field("code").value().toString());
        datum->set3TransformationParameters(rec.field("dx").value().toDouble(),
                                            rec.field("dy").value().toDouble(),
                                            rec.field("dz").value().toDouble());

        return datum;
    }

    return 0;
}
GeoReference *InternalIlwisObjectFactory::createGrfFromCode(const Resource& resource) const{
    QString code = resource.code();
    int index = code.indexOf(":");
    if ( index != -1)
        code = code.mid(index + 1);
    QStringList parts = code.split(",");
    std::map<QString, QString> parameters;
    for(auto part : parts){
       int index = part.indexOf("=");
       QString key = part.left(index).trimmed();
       QString value = part.mid(index + 1).trimmed();
       parameters[key] = value;
    }
    bool isCorners = parameters["type"] == "corners";
    GeoReference *cgrf = 0;
    ICoordinateSystem csy;
    Envelope env;
    Size<> sz;
    if ( isCorners){
       cgrf = new GeoReference(resource);
       cgrf->create("corners");
       cgrf->name(ANONYMOUS_PREFIX + QString::number(cgrf->id()));
    }
    if ( cgrf == 0)
        return 0;

    for(auto kvp : parameters){
        if ( kvp.first == "csy"){
            bool ok;
            quint64 id = kvp.second.toULongLong(&ok);
            if ( ok){
                csy .prepare(id);
            }else {
                QString name = kvp.second;
                if ( name.left(4) == "epsg" ||  name.left(5) == "proj4")
                    name = "code=" + name;
                csy.prepare(kvp.second);
            }
            if (!csy.isValid())
                return 0;
        }
        if ( kvp.first == "envelope"){
            QString coordstring = kvp.second;
            QStringList coords = coordstring.split(" ");
            if (!coords.size() == 4)
                return 0;
            bool ok1, ok2;
            env += Coordinate(coords[0].toDouble(&ok1), coords[1].toDouble(&ok2));
            if (!ok1 || !ok2)
                return 0;
            env += Coordinate(coords[2].toDouble(&ok1), coords[3].toDouble(&ok2));
            if (!ok1 || !ok2)
                return 0;
        }
        if ( kvp.first == "gridsize"){
            QStringList dims = kvp.second.split(" ");
            sz = Size<>(dims[0].toUInt(), dims[1].toUInt(),1);

        }
        if ( kvp.first == "name"){
            cgrf->name(kvp.second);
        }
        if ( kvp.first == "cornerofcorners")
            cgrf->centerOfPixel(kvp.second.compare("yes") != 0);
    }
    if ( parameters.find("name") == parameters.end())
        cgrf->name(ANONYMOUS_PREFIX + QString::number(cgrf->id()));
    if ( csy.isValid() && env.isValid() && sz.isValid()){
        cgrf->coordinateSystem(csy);
        cgrf->envelope(env);
        cgrf->size(sz);
        cgrf->compute();
        return cgrf;
    }
    return 0;
}

IlwisObject *InternalIlwisObjectFactory::createGeoreference(const Resource& resource, const IOOptions &options) const {
    GeoReference *cgrf;
    if ( resource.code() == "undetermined"){
        Resource resnew = resource;
        resnew.name(sUNDEF); // this will force a new object with a new id
        resnew.setId(i64UNDEF);
        cgrf = new GeoReference(resnew);
        cgrf->create("undetermined");
    } else if ( resource.code().indexOf("georef:") == 0){
        cgrf = createGrfFromCode(resource);
        if (cgrf == 0)
            ERROR2(ERR_ILLEGAL_VALUE_2,"georef code", resource.code());

    }else {
        cgrf = new GeoReference(resource);
        cgrf->create("corners");
        cgrf->name( resource["name"].toString());


        ICoordinateSystem csy;
        bool ok;
        if ( resource.hasProperty("coordinatesystem")){
            quint64 id = resource["coordinatesystem"].toULongLong(&ok);
            if ( ok) {
                csy = mastercatalog()->get(id);
            } else
                csy =  resource["coordinatesystem"].value<ICoordinateSystem>();

            cgrf->coordinateSystem(csy);
        }

        if ( resource.hasProperty("envelope"))
            cgrf->envelope(resource["envelope"].value<Envelope>());
        if ( resource.hasProperty("size"))
            cgrf->size(resource["size"].value<Size<>>());
        if ( resource.hasProperty("centerofpixel"))
            cgrf->centerOfPixel(resource["centerofpixel"].toBool());
    }

    if ( cgrf){

        cgrf->createTime(Time::now());
        cgrf->modifiedTime(Time::now());
        const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
        Resource res= resource;
        QString url = "ilwis://internalcatalog/" + cgrf->name();
        res.setUrl(url);
        res.setUrl(url,true);
        res.name(cgrf->name());
        ConnectorInterface *connector = factory->createFromResource<>(res, "internal");
        cgrf->setConnector(connector, IlwisObject::cmINPUT, options);
    }

    return cgrf;
}

IlwisObject *InternalIlwisObjectFactory::createTable(const Resource& resource, const IOOptions &options) const {

    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    ConnectorInterface *connector = factory->createFromResource<>(resource, "internal");

   if(!connector) {
       kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
       return 0;
   }
    Table *tbl;
    tbl = new FlatTable(resource);
    tbl->setConnector(connector, IlwisObject::cmINPUT, options);
    return tbl;

}
IlwisObject *InternalIlwisObjectFactory::createEllipsoid(const Resource& resource, const IOOptions &options) const {
    QString query    ;
    QString code = resource.code();

    if ( code != sUNDEF) {
        query = QString("Select * from ellipsoid where code = '%1'").arg(code);

    }else {
        QUrlQuery queryItem(resource.url());
        if ( queryItem.hasQueryItem("wkt")) {

            QString wkt = queryItem.queryItemValue("wkt");
            if ( wkt != "") {
                query = QString("Select * from ellipsoid where wkt = '%1'").arg(wkt);
            }

        }
    }
    return createEllipsoidFromQuery(query, resource);
}

IlwisObject *InternalIlwisObjectFactory::createEllipsoidFromQuery(const QString &query, const Resource& resource) const {
    if ( query == "") {
        return new Ellipsoid(resource);
    }

    InternalDatabaseConnection db;
    if (db.exec(query) && db.next()) {
        Ellipsoid *ellipsoid = new Ellipsoid(resource);
        ellipsoid->fromInternal(db.record());
        return ellipsoid;
    }
    return 0;
}

