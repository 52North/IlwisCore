#include "kernel.h"
#include "drawerattributesetterfactory.h"

using namespace Ilwis;
using namespace Geodrawer;

std::map<QString, DASCreator> DrawerAttributeSetterFactory::_creators;

DrawerAttributeSetterFactory::DrawerAttributeSetterFactory() : AbstractFactory("DrawerAttributeSetterFactory","UI", "Creates objects that are able to tell how a element is being drawn")
{

}

DrawerAttributeSetterFactory::~DrawerAttributeSetterFactory()
{

}

DrawerAttributeSetter *Ilwis::Geodrawer::DrawerAttributeSetterFactory::registerDrawerAttributeSetter(const QString &name, DASCreator createFunc)
{
    auto iter = _creators.find(name.toLower());
    if ( iter == _creators.end()){
        _creators[name.toLower()] = createFunc;
    }
    return 0;
}
