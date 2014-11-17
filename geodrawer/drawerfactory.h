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

typedef std::function<DrawerInterface *(const QString &name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer)> CreateDrawer;

class DrawerFactory : public AbstractFactory
{
public:
    DrawerFactory();
    static DrawerInterface * registerDrawer(const QString& name, CreateDrawer createFunc);
    static DrawerInterface * create(const QString& name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer);

private:
    static std::map<QString, CreateDrawer>  _creators;
};
}
}

#endif // DRAWERFACTORY_H
