#include <QtPlugin>
#include <QScopedPointer>
#include <functional>

#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "geos/geom/Geometry.h"
#include "range.h"
#include "domain.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "coordinatesystem.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "table.h"
#include "catalog.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "ilwisobjectconnector.h"
#include "symboltable.h"
#include "wfsmodule.h"
#include "wfsparsingcontext.h"
#include "wfsfeatureconnector.h"
#include "catalogconnector.h"
#include "catalogexplorer.h"
#include "wfscatalogexplorer.h"
#include "ilwisobjectfactory.h"
#include "wfsobjectfactory.h"
#include "wfsresponse.h"
#include "wfs.h"

using namespace Ilwis;
using namespace Wfs;

WfsModule::WfsModule(QObject *parent) :
    Module(parent, "WfsModule", "iv40","1.0")
{
}

WfsModule::~WfsModule()
{
}

QString WfsModule::getInterfaceVersion() const
{
    return "iv40";
}

void WfsModule::prepare()
{
    WfsObjectFactory *factory = new WfsObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory);

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return;


    cfactory->addCreator(itCATALOG, "wfs", CatalogConnector::create);
    cfactory->addCreator(itFEATURE, "wfs", WfsFeatureConnector::create);

    kernel()->issues()->log("Loaded wfs module",IssueObject::itMessage);
}

QString WfsModule::getName() const
{
    return "WFS plugin";
}

QString WfsModule::getVersion() const
{
    return "1.0";
}

