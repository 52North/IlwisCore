#include <QtPlugin>
#include <QSqlQuery>

#include "kernel.h"
#include "ilwis.h"
#include "geos/geom/Coordinate.h"
#include "coordinate.h"
#include "location.h"
#include "projectionmodule.h"
#include "factory.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "projectionfactory.h"
#include "prjmplfactoryproj4.h"

using namespace Ilwis;

ProjectionModule::ProjectionModule(QObject *parent) :
    Module(parent, "ProjectionModuleProj4", "iv40","1.0")
{
}

QString ProjectionModule::getInterfaceVersion() const
{
    return "iv40";

}

void Ilwis::ProjectionModule::prepare()
{
    FactoryInterface *factory = new ProjectionImplFactoryProj4();
    factory->prepare();
    kernel()->addFactory(factory );
}

QString ProjectionModule::name() const
{
    return "Projection connector plugin (Proj4)";
}

QString ProjectionModule::version() const
{
    return "1.0";
}




