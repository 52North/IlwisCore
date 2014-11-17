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
    auto iter = _creators.find(name);
    if ( iter == _creators.end()){
        _creators[name] = createFunc;
    }
    return 0;
}

DrawerInterface *DrawerFactory::create(const QString &name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer)
{
   auto iter = _creators.find(name);
   if ( iter == _creators.end()){
       return 0;
   }
   return (*iter).second(name, parentDrawer, rootdrawer);
}
