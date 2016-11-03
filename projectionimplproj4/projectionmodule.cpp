#include <QtPlugin>
#include <QSqlQuery>

#include "kernel.h"
#include "ilwis.h"
#include "geos/geom/Coordinate.h"
#include "ilwiscoordinate.h"
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

void Ilwis::ProjectionModule::prepare()
{
    FactoryInterface *factory = new ProjectionImplFactoryProj4();
    factory->prepare();
    kernel()->addFactory(factory );

    kernel()->issues()->log("Loaded Proj4 module",IssueObject::itMessage);
}




