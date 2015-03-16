#ifndef DRAWERFACTORY_H
#define DRAWERFACTORY_H

#include <functional>
#include "factory.h"
#include "abstractfactory.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
namespace Geodrawer{

class RootDrawer;
class DrawerInterface;
typedef std::unique_ptr<RootDrawer> UPRootDrawer;
typedef std::unique_ptr<DrawerInterface> UPDrawer;

typedef std::function<DrawerInterface *(DrawerInterface* parentDrawer, RootDrawer *rootdrawer, const IOOptions &options)> CreateDrawer;

class ILWISCOREUISHARED_EXPORT DrawerFactory : public AbstractFactory
{
public:
    DrawerFactory();
    static DrawerInterface * registerDrawer(const QString& name, CreateDrawer createFunc);
    //static DrawerInterface * create(const QString& name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer);

    template<class T=DrawerInterface> static T *create(const QString &name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options)
    {
       auto iter = _creators.find(name.toLower());
       if ( iter == _creators.end()){
           return 0;
       }
       return static_cast<T *>((*iter).second(parentDrawer, rootdrawer, options));
    }

    template<class T=DrawerInterface> static T *create(IlwisTypes tp,
                                                       DrawerInterface *parentDrawer,
                                                       RootDrawer *rootdrawer,
                                                       const IOOptions &options, const QString& subType="")
    {
        QString typeName = ilwisType2DrawerName(tp, subType);
        return create<T>(typeName,parentDrawer,rootdrawer,options);
    }

private:
    static std::map<QString, CreateDrawer>  _creators;

    static QString ilwisType2DrawerName(IlwisTypes tp, const QString& subType);
};
}
}

#endif // DRAWERFACTORY_H
