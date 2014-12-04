#include "coverage.h"
#include "geodrawer.h"
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include "rootdrawer.h"
#include "layerdrawer.h"
#include "complexdrawer.h"
#include "drawingcolor.h"
#include "drawerfactory.h"

using namespace Ilwis;

GeoDrawer::GeoDrawer(QQuickItem *parent):
    QQuickItem(parent)
{
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
    _rootDrawer = new Ilwis::Geodrawer::RootDrawer(this);

}

void GeoDrawer::addDataSource(const QString &url, const QString& typeName)
{
    IlwisTypes tp = Ilwis::IlwisObject::name2Type(typeName);
    if ( tp == itUNKNOWN)
        return;

    for(auto resource : _datasources)    {
        if ( resource.url().toString() == url && resource.ilwisType() == tp)
            return;
    }

    quint64 id = mastercatalog()->url2id(QUrl(url),tp);
    if ( id == i64UNDEF){
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, TR("Visualization"), url);
        return ;
    }
    Resource resource = mastercatalog()->id2Resource(id);
    if (! resource.isValid()){
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, TR("Visualization"), url);
        return ;
    }

    _datasources.push_back(resource);
    ICoverage coverage(resource);
    if ( !coverage.isValid()){
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, TR("Visualization"), url);
        return;
    }
    Geodrawer::LayerDrawer *drawer = Geodrawer::DrawerFactory::create<Geodrawer::LayerDrawer>(coverage->ilwisType(), _rootDrawer, _rootDrawer);
    if ( !drawer)
        return;

    drawer->coverage(coverage);
    Size<> sz(width(), height(), 1);
    _rootDrawer->pixelAreaSize(sz);
    if ( _rootDrawer->drawerCount(Geodrawer::ComplexDrawer::dtMAIN) == 0)
        _rootDrawer->coordinateSystem(coverage->coordinateSystem());
    _rootDrawer->addDrawer(drawer,false);
}

GeoDrawer::~GeoDrawer()
{
    cleanup();
}

void GeoDrawer::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, SIGNAL(afterRendering()), this, SLOT(paint()), Qt::DirectConnection);
        connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
       // win->setClearBeforeRendering(false);
    }
}

void GeoDrawer::paint()
{
    try {
        if ( !_rootDrawer)
            return;

        if ( !_rootDrawer->isActive())
            return ;

        int w = width();
        int h = height();
        int heightWindow = window()->contentItem()->height();
        _rootDrawer->pixelAreaSize(Ilwis::Size<>(w,h,0));
        QPointF pointInLocalCS(x(), y());
        QPointF pointInWindowCS = window()->contentItem()->mapFromItem(this, pointInLocalCS);
        int yb = heightWindow - h - pointInWindowCS.y() + pointInLocalCS.y();
        glViewport(pointInWindowCS.x() - pointInLocalCS.x(), yb, w, h);
        _rootDrawer->draw(window()->openglContext());

        glViewport(0,0,w, h);
    }
    catch (const ErrorObject&){
    }
    catch ( const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
    catch( ...){
        kernel()->issues()->log(TR("Recovering from unknown exception"));
    }

}

void GeoDrawer::cleanup()
{
    if ( _rootDrawer){
        _rootDrawer->cleanUp();
    }
}

void GeoDrawer::sync()
{
}



