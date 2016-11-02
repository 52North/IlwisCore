#ifndef COMPLEXDRAWER_H
#define COMPLEXDRAWER_H

#include "basedrawer.h"
#include "boost/container/flat_map.hpp"
#include "DrawerInterface.h"
#include "basedrawer.h"

namespace Ilwis {
namespace Geodrawer{

class DrawerInterface;

typedef std::map<quint32, std::unique_ptr<DrawerInterface>> DrawerMap;
typedef std::vector<std::unique_ptr<DrawerInterface>> DrawerList;

typedef std::unique_ptr<DrawerInterface> UPDrawer;

class RootDrawer;



class ILWISCOREUISHARED_EXPORT ComplexDrawer : public BaseDrawer
{
public:



    bool draw(const IOOptions& options=IOOptions()) ;
    bool prepare(DrawerInterface::PreparationType prepType, const IOOptions& options);
    void unprepare(DrawerInterface::PreparationType prepType);
    bool prepareChildDrawers(DrawerInterface::PreparationType prepType, const IOOptions& options);

    quint32 drawerCount(DrawerInterface::DrawerType tpe) const;
    const UPDrawer& drawer(quint32 order, DrawerInterface::DrawerType drawerType = DrawerInterface::dtMAIN) const;
    UPDrawer& drawer(quint32 order, DrawerInterface::DrawerType drawerType = DrawerInterface::dtMAIN) ;
    UPDrawer& drawer(const QString& namecode, DrawerInterface::DrawerType drawerType = DrawerInterface::dtMAIN) ;
    const UPDrawer& drawer(const QString& namecode, DrawerInterface::DrawerType drawerType = DrawerInterface::dtMAIN) const;
    void addDrawer(DrawerInterface *drawer, DrawerInterface::DrawerType drawerType = DrawerInterface::dtMAIN, quint32 order=iUNDEF, const QString& name=sUNDEF);
    void setDrawer(quint32 order, DrawerInterface *drawer, DrawerInterface::DrawerType tp = DrawerInterface::dtMAIN);
    void removeDrawer(quint32 order, DrawerInterface::DrawerType drawerType = DrawerInterface::dtMAIN);
    void removeDrawer(const QString& idcode, bool ascode);

    bool isSimple() const;
    bool hasDrawer(const QString& namecode, DrawerInterface::DrawerType drawerType = DrawerInterface::dtMAIN) const;

     void cleanUp();

    bool drawerAttribute(const QString& drawercode, const QString& key, const QVariant& value);
    std::vector<QVariant> attributes(const QString &attrNames) const;
    QVariant attribute(const QString &attrName) const;
    QVariant attributeOfDrawer(const QString& drawercode, const QString& attrName) const;
    QVariant execute(const QString &operationName, const QVariantMap &parameters);
protected:
    ComplexDrawer(const QString &name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);

private:
    DrawerMap _preDrawers;
    DrawerList _mainDrawers;
    DrawerMap _postDrawers;

    bool drawSideDrawers(const DrawerMap& drawers, const IOOptions& options) const;


};
}
}

#endif // COMPLEXDRAWER_H
