#include "kernel.h"
#include "drawerfactory.h"

using namespace Ilwis;
using namespace Geodrawer;

std::map<QString, CreateDrawer>  DrawerFactory::_creators;

DrawerFactory::DrawerFactory() : AbstractFactory("DrawerFactory","ilwis","Creates the geo drawers of various types that draw spatail data")
{
}


DrawerInterface *DrawerFactory::registerDrawer(const QString &name, Ilwis::Geodrawer::CreateDrawer createFunc)
{
    auto iter = _creators.find(name.toLower());
    if ( iter == _creators.end()){
        _creators[name.toLower()] = createFunc;
    }
    return 0;
}

QString DrawerFactory::ilwisType2DrawerName(IlwisTypes tp, const QString &subType)
{
    //translates names to common drawers; note that this only will handle the translations
    //that can be expected to be present on an ilwis system. not all names will be mapped
    if ( subType != "Animation"){
        if ( hasType(tp, itFEATURE))
            return "FeatureLayerDrawer";
        if ( hasType(tp, itRASTER)){
            return "RasterLayerDrawer" ;
        }
        if ( hasType(tp, itLINE)){
            return "FeatureLineDrawer";
        }
        if ( hasType(tp, itPOLYGON)){
            return "FeaturePolygonDrawer";
        }
        if ( hasType(tp, itPOINT)){
            return "FeaturePointDrawer";
        }
    }
    return sUNDEF;
}


