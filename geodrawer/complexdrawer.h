#ifndef COMPLEXDRAWER_H
#define COMPLEXDRAWER_H

#include "basedrawer.h"

namespace Ilwis {
namespace Geodrawer{

typedef std::map<quint32, std::unique_ptr<DrawerInterface>> DrawerMap;
typedef std::vector<std::unique_ptr<DrawerInterface>> DrawerVector;

class RootDrawer;


class ComplexDrawer : public BaseDrawer
{
public:
    enum DrawerType{dtDONTCARE=0, dtPOST=1, dtPRE=2, dtMAIN=4, dtALL=0xFFFFFFFF};



    bool draw(QOpenGLContext *openglContext, const IOOptions& options=IOOptions()) ;
    bool prepare(PreparationType prepType, const IOOptions& options,QOpenGLContext *openglContext = 0);
    void unprepare(PreparationType prepType);
    bool prepareChildDrawers(PreparationType prepType, const IOOptions& options);

    quint32 drawerCount(ComplexDrawer::DrawerType tpe) const;
    const UPDrawer& drawer(quint32 order, ComplexDrawer::DrawerType drawerType = dtMAIN) const;
    void addDrawer(DrawerInterface *drawer, ComplexDrawer::DrawerType drawerType = dtMAIN, quint32 order=iUNDEF);
    void setDrawer(quint32 order, DrawerInterface *drawer, ComplexDrawer::DrawerType tp = dtMAIN);
    void removeDrawer(quint32 order, ComplexDrawer::DrawerType drawerType = dtMAIN);

    bool isSimple() const;

     void cleanUp();

    std::vector<VertexPosition>& drawPositions();

    std::vector<QVariant> attributes(const QString &attrNames) const;
    QVariant attribute(const QString &attrName) const;
protected:
    ComplexDrawer(const QString &name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer, QObject *parent=0);

private:
    DrawerMap _preDrawers;
    DrawerVector _mainDrawers;
    DrawerMap _postDrawers;


    bool drawSideDrawers(QOpenGLContext *openglContext, const DrawerMap& drawers, const IOOptions& options) const;


};
}
}

#endif // COMPLEXDRAWER_H
