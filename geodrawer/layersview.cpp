#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include "visualizationmanager.h"
#include "coverage.h"
#include "layersview.h"
#include "layersrenderer.h"
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
    QVariant var = _copiedAttributes.take(drawercode.toLower() + "|" + attrName.toLower());
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
    if ( tpName == "Ilwis::BoubdingBox"){
        auto bb = var.value<BoundingBox>();
        return bb.toString();
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

QString LayersView::viewerId() const
{
    return QString::number(_viewerId);
}


