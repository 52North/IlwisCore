#include <QtPlugin>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "kernel.h"
#include "geos/geom/Coordinate.h"
#include "ilwiscoordinate.h"
#include "location.h"
#include "internalmodule.h"
#include "ilwisdata.h"
#include "range.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"
#include "internalilwisobjectfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "internalrastercoverageconnector.h"
#include "internalfeaturecoverageconnector.h"
#include "internaltableconnector.h"
#include "workflowconnector.h"
#include "operationmetadataconnector.h"
#include "projection.h"
#include "projectionimplementation.h"
#include "projectionfactory.h"
#include "internalprjmplfactory.h"
#include "internalgeoreferenceconnector.h"
#include "internaldomain.h"
#include "internalcoordinatesystemconnector.h"
#include "internalrepresentation.h"
#include "internalprojectionconnector.h"
#include "internalellipsoidconnector.h"
#include "internalscriptconnector.h"
#include "internalmodelconnector.h"
#include "internalcombinationmatrixconnector.h"
#include "ilwiscontext.h"

#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"

using namespace Ilwis;
using namespace Internal;

InternalModule::InternalModule(QObject *parent) :
    Module(parent, "InternalModule", "iv40","1.0")
{
}

QString InternalModule::getInterfaceVersion() const
{
    return "iv40";

}

void InternalModule::prepare()
{
    InternalIlwisObjectFactory *ifactory = new InternalIlwisObjectFactory();
    kernel()->addFactory(ifactory);

    ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!factory)
        return ;
    factory->addCreator(itCATALOG,"internal",CatalogConnector::create);
    factory->addCreator(itRASTER,"internal", InternalRasterCoverageConnector::create);
    factory->addCreator(itTABLE,"internal", InternalTableConnector::create);
    factory->addCreator(itFEATURE,"internal", InternalFeatureCoverageConnector::create);
    factory->addCreator(itGEOREF,"internal", InternalGeoReferenceConnector::create);
    factory->addCreator(itDOMAIN,"internal", InternalDomainConnector::create);
    factory->addCreator(itCOORDSYSTEM,"internal", InternalCoordinatesystemConnector::create);
    factory->addCreator(itREPRESENTATION,"internal", InternalRepresentationConnector::create);
    factory->addCreator(itWORKFLOW,"internal", WorkflowConnector::create);
    factory->addCreator(itMODEL,"internal", InternalModelConnector::create);
    factory->addCreator(itSINGLEOPERATION,"internal", OperationMetadataConnector::create);
    factory->addCreator(itPROJECTION,"internal", InternalProjectionConnector::create);
    factory->addCreator(itELLIPSOID,"internal", InternalEllipsoidConnector::create);
    factory->addCreator(itSCRIPT,"internal", InternalScriptConnector::create);
    factory->addCreator(itCOMBINATIONMATRIX,"internal", InternalCombinationMatrixConnector::create);

    FactoryInterface *projfactory = new ProjectionImplFactory();
    projfactory->prepare();
    kernel()->addFactory(projfactory );

    InternalDatabaseConnection db;

    bool ok = createItems(db,"projection", "projections",itPROJECTION);
    ok &= createItems(db,"ellipsoid", "ellipsoids",itELLIPSOID);
    ok &= createItems(db,"datum", "datums",itGEODETICDATUM);
    ok &= createItems(db,"numericdomain", "domains", itNUMERICDOMAIN, "domain");
    ok &= createItems(db,"representation", "representations",itREPRESENTATION);
    ok &= createPcs(db);
    ok &= createSpecialDomains();

    QString url = QString("ilwis://system/coordinatesystems/unknown");
    Resource resource(url, itBOUNDSONLYCSY);
    resource.code("unknown");
    resource.name("unknown", false);
    resource.addContainer(QUrl("ilwis://system/coordinatesystems"));
    resource.prepare();
    mastercatalog()->addItems({resource});

    url = QString("ilwis://system/georefs/undetermined");
    resource = Resource(url, itGEOREF);
    resource.code("undetermined");
    resource.name("undetermined", false);
    resource.addContainer(QUrl("ilwis://system/georefs"));
    resource.prepare();
    mastercatalog()->addItems({resource});

//    url = QString("file:///%1/resources/country_boundaries.ilwis").arg(context()->ilwisFolder().absoluteFilePath());
//    resource = Resource(QUrl("ilwis://system/country_boundaries.ilwis"), QUrl(url),itFEATURE);
//    resource.code("catalogcountryboundaries");
//    resource.name("countryboundaries", false);
//    resource.addContainer(QUrl("ilwis://system"));
//    resource.prepare();
//    mastercatalog()->addItems({resource});


    IlwisObject::addTypeFunction(InternalModule::ilwisType);

    kernel()->issues()->log("Loaded internal objects module",IssueObject::itMessage);

}

QString InternalModule::name() const
{
    return "Internal Connector connector plugin";
}

QString InternalModule::version() const
{
    return "1.0";
}

bool InternalModule::createSpecialDomains() {
    std::vector<Resource> resources;
    QString url = QString("ilwis://system/domains/code=domain:text");
    Resource resource(url, itTEXTDOMAIN);
    resource.code("text");
    resource.name("Text domain", false);
    resource.addContainer(QUrl("ilwis://system/domains"));
    resources.push_back(resource);

    url = QString("ilwis://system/domains/code=domain:color");
    Resource colorResource(url, itCOLORDOMAIN);
    colorResource.code("color");
    colorResource.name("Color domain", false);
    colorResource.addContainer(QUrl("ilwis://system/domains"));
    resources.push_back(colorResource);

    url = QString("ilwis://system/domains/code=domain:colorpalette");
    Resource paletteResource(url, itITEMDOMAIN);
    paletteResource.code("colorpalette");
    paletteResource.name("Color Palette domain", false);
    paletteResource.addContainer(QUrl("ilwis://system/domains"));
    resources.push_back(paletteResource);

    InternalDatabaseConnection itemdomainTable;
    QString query = "Select * from itemdomain";
    if (itemdomainTable.exec(query)){
        while (itemdomainTable.next()) {
             QSqlRecord rec = itemdomainTable.record();
             QString code = rec.value("code").toString();
             QString url = QString("ilwis://system/domains/%1").arg(code);
             Resource res(url, itITEMDOMAIN);
             res.code(code);
             res.name(rec.value("name").toString());
             res.addContainer({"ilwis://system/domains"});
             res.modifiedTime(Time::now());
             resources.push_back(res);
        }
    }

    return mastercatalog()->addItems(resources);
}

bool InternalModule::createPcs(InternalDatabaseConnection &db) {
    QString query = QString("Select * from projectedcsy");
    if ( db.exec(query)) {
        std::vector<Resource> items;
        while (db.next()) {
            QSqlRecord rec = db.record();
            QString code = rec.value("code").toString();
            QString name = rec.value("name").toString();
            QString url = QString("ilwis://system/coordinatesystems/%1").arg(code);
            Resource resource(url, itCONVENTIONALCOORDSYSTEM);
            resource.code(code);
            resource.name(name, false);
            resource["wkt"] = name;
            resource.addContainer(QUrl("ilwis://system/coordinatesystems"));
            items.push_back(resource);
        }
        bool ok = mastercatalog()->addItems(items);
        return ok;

    } else {
        kernel()->issues()->logSql(db.lastError());
    }
    return false;
}

bool InternalModule::createItems(InternalDatabaseConnection &db, const QString& sqltable, const QString& folder, IlwisTypes type, const QString internalname) {
    QString query = QString("Select * from %1").arg(sqltable);
    if ( db.exec(query)) {
        std::vector<Resource> items;
        while (db.next()) {
            QSqlRecord rec = db.record();
            QString code = rec.value("code").toString();
            IlwisTypes extType = rec.value("extendedtype").toLongLong();
            QString table = internalname == "" ? sqltable : internalname;
            QString url = QString("ilwis://system/%1/%2").arg(folder,code);
            Resource resource(url, type);
            if ( hasType(type, itNUMERICDOMAIN | itREPRESENTATION)) // for valuedomain name=code
                resource.name(rec.value("code").toString(), false);
            else
                resource.name(rec.value("name").toString(), false);

            resource.code(code);
            resource.setExtendedType(extType);
            resource.setDescription(rec.value("description").toString());
            resource.addContainer(QUrl("ilwis://system/" + folder));
            QString wkt = rec.value("wkt").toString();
            if ( wkt != "" && wkt != sUNDEF)
                resource.addProperty("wkt",wkt);
            QString domname = rec.value("relateddomain").toString();
            if ( domname != "" && domname != sUNDEF)
                resource.addProperty("relateddomain", domname);
            items.push_back(resource);
        }
        return mastercatalog()->addItems(items);
    } else {
        kernel()->issues()->logSql(db.lastError());
    }
    return false;
}

IlwisTypes InternalModule::ilwisType(const QString &name) {
    if ( name == sUNDEF)
        return itUNKNOWN;
    bool ok;
    name.toDouble(&ok); // numbers are not handled here
    if ( ok)
        return false;
    if ( name.size() > 0 && name[0] == '\"') // quoted strings
        return itSTRING;

    auto FindType = [](InternalDatabaseConnection &dbi, const QString& code, const QString& sqltable)->bool{
        QString query = QString("Select code from %2 where code='%1'").arg(code).arg(sqltable);
        if ( dbi.exec(query))
            return dbi.next();
        return false;
    };
    InternalDatabaseConnection db;
    if ( name.indexOf("/") == -1){ // no paths in codes
        if ( FindType(db, name, "numericdomain"))
            return itDOMAIN;
        if ( FindType(db, name, "itemdomain"))
            return itDOMAIN;
        else if ( FindType(db, name, "projection"))
            return itPROJECTION;
        else if ( FindType(db, name, "projectedcsy"))
            return itCOORDSYSTEM;
        else if ( FindType(db, name, "ellipsoid"))
            return itELLIPSOID;
        else if ( FindType(db, name, "representation"))
            return itREPRESENTATION;
        else if ( FindType(db, name, "datum"))
            return itGEODETICDATUM;
    }


    QString objectlocation = name;
    if ( !name.contains(QRegExp("\\\\|/")))    {
        objectlocation = INTERNAL_CATALOG + "/" + name;
    }
    Resource res = mastercatalog()->name2Resource(objectlocation);
    return res.ilwisType();
}






