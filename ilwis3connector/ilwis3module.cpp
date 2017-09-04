#include <QtPlugin>
#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QFileInfo>
#include <QSize>
#include <fstream>

#include "kernel.h"

#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"

#include "raster.h"
#include "connectorinterface.h"
#include "numericrange.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "factory.h"
#include "inifile.h"
#include "ilwis3connector.h"
#include "columndefinition.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "table.h"
#include "pixeliterator.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwis3module.h"
#include "ilwisobjectfactory.h"
#include "ilwis3objectfactory.h"
#include "catalog.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "foldercatalogexplorer.h"
#include "ilwis3catalogexplorer.h"
#include "rawconverter.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "ellipsoidconnector.h"
#include "coordinatesystemconnector.h"
#include "georefconnector.h"
#include "coverageconnector.h"
#include "ilwisrastercoverageconnector.h"
#include "domainconnector.h"
#include "tableconnector.h"
#include "ilwis3projectionconnector.h"
#include "domainconnector.h"
#include "ilwiscontext.h"
#include "dataformat.h"
#include "ilwis3featureconnector.h"
#include "representation.h"
#include "ilwis3representationconnector.h"


using namespace Ilwis;
using namespace Ilwis3;

Ilwis3Module::Ilwis3Module(QObject *parent) :
    Module(parent, "IlwisModule", "iv40","1.0")
{
}

QString Ilwis3Module::getInterfaceVersion() const
{
    return "iv40";

}

void Ilwis3Module::prepare()
{
    Ilwis3ObjectFactory *objfactory = new Ilwis3ObjectFactory();
    objfactory->prepare();
    kernel()->addFactory(objfactory );

    ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!factory)
        return ;
    factory->addCreator(itITEMDOMAIN | itNUMERICDOMAIN ,"ilwis3", Ilwis3::DomainConnector::create);
    factory->addCreator(itTABLE,"ilwis3",Ilwis3::TableConnector::create);
    factory->addCreator(itRASTER,"ilwis3", Ilwis3::RasterCoverageConnector::create);
    factory->addCreator(itGEOREF,"ilwis3", Ilwis3::GeorefConnector::create);
    factory->addCreator(itPROJECTION,"ilwis3", Ilwis3::ProjectionConnector::create);
    factory->addCreator(itELLIPSOID,"ilwis3",Ilwis3::EllipsoidConnector::create);
    factory->addCreator(itCOORDSYSTEM,"ilwis3",Ilwis3::CoordinateSystemConnector::create);
    factory->addCreator(itFEATURE,"ilwis3",Ilwis3::FeatureConnector::create);
    factory->addCreator(itREPRESENTATION,"ilwis3",Ilwis3::Ilwis3RepresentationConnector::create);

    factory->addCreator("map","ilwis3",Ilwis3::RasterCoverageConnector::create);
    factory->addCreator("georef","ilwis3",Ilwis3::GeorefConnector::create);
    factory->addCreator("georeference","ilwis3",Ilwis3::GeorefConnector::create);
    factory->addCreator("vectormap","ilwis3",Ilwis3::FeatureConnector::create);
    factory->addCreator("polygonmap","ilwis3",Ilwis3::FeatureConnector::create);
    factory->addCreator("linemap","ilwis3",Ilwis3::FeatureConnector::create);
    factory->addCreator("pointmap","ilwis3",Ilwis3::FeatureConnector::create);
    factory->addCreator("domain","ilwis3",Ilwis3::DomainConnector::create);
    factory->addCreator("coordsystem","ilwis3",Ilwis3::CoordinateSystemConnector::create);
    factory->addCreator("table","ilwis3",Ilwis3::TableConnector::create);
    factory->addCreator("representation","ilwis3",Ilwis3::Ilwis3RepresentationConnector::create);

    IlwisObject::addTypeFunction(Ilwis3Connector::ilwisType);

    QFileInfo ilw = context()->ilwisFolder();
    QString path = ilw.canonicalFilePath() + "/extensions/ilwis3connector/resources";
    DataFormat::setFormatInfo(path + "/ilwis3formats.config","ilwis3");

    kernel()->issues()->log("Loaded module Ilwis3 connector",IssueObject::itMessage);


}

QString Ilwis3Module::name() const
{
    return "Ilwis 3 connector plugin";
}

QString Ilwis3Module::version() const
{
    return "1.0";
}

void Ilwis3Module::getOperations(QVector<ICommandInfo *> &) const{
}





//Q_EXPORT_PLUGIN2(Ilwis3Connector, Ilwis::Ilwis3::Ilwis3Module)
