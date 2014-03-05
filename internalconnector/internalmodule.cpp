#include <QtPlugin>
#include <QSqlQuery>

#include "kernel.h"
#include "geos/geom/Coordinate.h"
#include "coordinate.h"
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
#include "catalogconnector.h"
#include "internalcatalogconnector.h"
#include "internalrastercoverageconnector.h"
#include "internalfeaturecoverageconnector.h"
#include "internaltableconnector.h"
#include "projection.h"
#include "ProjectionImplementation.h"
#include "projectionfactory.h"
#include "internalprjmplfactory.h"

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
    factory->addCreator(itCATALOG,"internal",InternalCatalogConnector::create);

    factory->addCreator(itRASTER,"internal", InternalRasterCoverageConnector::create);
    factory->addCreator(itTABLE,"internal", InternalTableConnector::create);
    factory->addCreator(itFEATURE,"internal", InternalFeatureCoverageConnector::create);

    FactoryInterface *projfactory = new ProjectionImplFactory();
    projfactory->prepare();
    kernel()->addFactory(projfactory );


}

QString InternalModule::name() const
{
    return "Internal Connector connector plugin";
}

QString InternalModule::version() const
{
    return "1.0";
}






