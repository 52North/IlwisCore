#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include "visualizationmanager.h"
#include "coverage.h"
#include "layersview.h"
#include "layersrenderer.h"
#include "rootdrawer.h"
#include "uicontextmodel.h"

#include <QtGui/QOpenGLFramebufferObject>

#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>

quint64 LayersView::_baseViewerId = 0;

LayersView::LayersView()
{
    _viewerId = _baseViewerId++;
    uicontext()->addViewer(this, _viewerId);
}

LayersView::~LayersView()
{
    uicontext()->removeViewer(_viewerId)    ;
}

QQuickFramebufferObject::Renderer *LayersView::createRenderer() const
{
    return new LayersRenderer();
}

void LayersView::addCommand(const QString &command, const QVariantMap &params)
{

}

void LayersView::setAttribute(const QString &drawercode, const QVariantMap &values)
{
    _attributeQueue.push_back(std::pair<QString, QVariantMap>(drawercode, values));

}

void LayersView::copyAttribute(const QString &drawercode, const QString &attrName)
{
    _attributerequests.push_back(std::pair<QString, QString>(drawercode, attrName));
}

QString LayersView::attributeOfDrawer(const QString &drawercode, const QString &attrName)
{
    try {
        Ilwis::Geodrawer::RootDrawer *rootdrawer = rootDrawer();
        if ( !rootdrawer)
            return "";

        QVariant var = rootdrawer->attributeOfDrawer(drawercode, attrName);
        if ( !var.isValid())
            return "";
        QString result = var.toString();
        if ( result != "")
            return result;
        QString tpName = var.typeName();
        if ( tpName == "Ilwis::Envelope"){
            Envelope env = var.value<Envelope>();
            QString result = env.toString();
            return result;
        }
        if ( tpName == "Ilwis::BoundingBox"){
            auto bb = var.value<BoundingBox>();
            return bb.toString();
        }

        return "";
    } catch ( const ErrorObject&){

    } catch ( std::exception& ex){

    }
    return "";
}

void LayersView::addCommand(const QString &expression)
{
    OperationExpression expr(expression);
    if ( expr.isValid()){
        _commands.push_front(expr);
    }
}

void LayersView::setManager(LayerManager *manager)
{
    _manager = manager;
}

LayerManager *LayersView::layerManager()
{
    return _manager;
}

QString LayersView::viewerId() const
{
    return QString::number(_viewerId);
}

Geodrawer::RootDrawer *LayersView::rootDrawer()
{
    if ( !_manager)
        return 0;
    CoverageLayerModel *layer = _manager->layer(1); // layer 0 is the global layer, no 'real' drawer there
    if ( !layer)
        return 0;
    return layer->drawer()->rootDrawer();
}

Ilwis::ICoordinateSystem LayersView::rootCoordinateSystem() {
    if ( rootDrawer()){
        return rootDrawer()->coordinateSystem();
    }
    return Ilwis::ICoordinateSystem();
}


