#ifndef COMPLEXDRAWER_H
#define COMPLEXDRAWER_H

#include "basedrawer.h"
#include "boost/container/flat_map.hpp"

namespace Ilwis {
namespace Geodrawer{

typedef std::map<quint32, std::unique_ptr<DrawerInterface>> DrawerMap;
typedef std::vector<std::unique_ptr<DrawerInterface>> DrawerList;

class RootDrawer;


class ComplexDrawer : public BaseDrawer
{
public:



    bool draw(const IOOptions& options=IOOptions()) ;
    bool prepare(PreparationType prepType, const IOOptions& options);
    void unprepare(PreparationType prepType);
    bool prepareChildDrawers(PreparationType prepType, const IOOptions& options);

    quint32 drawerCount(ComplexDrawer::DrawerType tpe) const;
    const UPDrawer& drawer(quint32 order, DrawerInterface::DrawerType drawerType = dtMAIN) const;
    void addDrawer(DrawerInterface *drawer, DrawerInterface::DrawerType drawerType = dtMAIN, quint32 order=iUNDEF, const QString& name=sUNDEF);
    void setDrawer(quint32 order, DrawerInterface *drawer, DrawerInterface::DrawerType tp = dtMAIN);
    void removeDrawer(quint32 order, DrawerInterface::DrawerType drawerType = dtMAIN);
    void removeDrawer(const QString& idcode, bool ascode);

    bool drawerAttribute(const QString& drawercode, const QString& key, const QVariant& value);

    bool isSimple() const;

     void cleanUp();


    std::vector<QVariant> attributes(const QString &attrNames) const;
    QVariant attribute(const QString &attrName) const;
    QVariant attributeOfDrawer(const QString& drawercode, const QString& attrName) const;
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
