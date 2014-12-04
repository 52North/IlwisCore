#ifndef DRAWERFACTORY_H
#define DRAWERFACTORY_H

#include <functional>
#include "factory.h"
#include "abstractfactory.h"

namespace Ilwis {
namespace Geodrawer{

class RootDrawer;
class DrawerInterface;
typedef std::unique_ptr<RootDrawer> UPRootDrawer;
typedef std::unique_ptr<DrawerInterface> UPDrawer;

typedef std::function<DrawerInterface *(DrawerInterface* parentDrawer, RootDrawer *rootdrawer)> CreateDrawer;

class DrawerFactory : public AbstractFactory
{
public:
    DrawerFactory();
    static DrawerInterface * registerDrawer(const QString& name, CreateDrawer createFunc);
    static DrawerInterface * create(const QString& name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer);

    template<class T=DrawerInterface> static T *create(const QString &name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer)
    {
       auto iter = _creators.find(name);
       if ( iter == _creators.end()){
           return 0;
       }
       return static_cast<T *>((*iter).second(parentDrawer, rootdrawer));
    }

    template<class T=DrawerInterface> static T *create(IlwisTypes tp, DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const QString& subType="")
    {
        QString typeName = ilwisType2DrawerName(tp, subType);
        return create<T>(typeName,parentDrawer,rootdrawer);
    }

private:
    static std::map<QString, CreateDrawer>  _creators;

    static QString ilwisType2DrawerName(IlwisTypes tp, const QString& subType);
};
}
}

#endif // DRAWERFACTORY_H
