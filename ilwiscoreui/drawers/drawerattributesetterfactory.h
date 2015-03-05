#ifndef DRAWERATTRIBUTESETTERFACTORY_H
#define DRAWERATTRIBUTESETTERFACTORY_H

#include <functional>
#include "factory.h"
#include "abstractfactory.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
namespace Geodrawer {

class DrawerAttributeSetter;

typedef std::function<DrawerAttributeSetter* (const IOOptions &options)> DASCreator;

class ILWISCOREUISHARED_EXPORT DrawerAttributeSetterFactory : public AbstractFactory
{
public:
    DrawerAttributeSetterFactory();
    ~DrawerAttributeSetterFactory();

    static DrawerAttributeSetter * registerDrawerAttributeSetter(const QString& name, DASCreator createFunc);

    template<class T=DrawerAttributeSetter> static T *create(const QString name, const IOOptions &options)
    {
       auto iter = _creators.find(name.toLower());
       if ( iter == _creators.end()){
           return 0;
       }
       return static_cast<T *>((*iter).second(options));
    }

private:
    static std::map<QString, DASCreator> _creators;
};
}
}

#endif // DRAWERATTRIBUTESETTERFACTORY_H
