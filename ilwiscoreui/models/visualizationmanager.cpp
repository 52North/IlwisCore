#include <QDebug>
#include <memory>
#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "iooptions.h"
#include "uicontextmodel.h"
#include "coveragelayermodel.h"
#include "visualizationmanager.h"

using namespace Ilwis;

LayerManager::LayerManager(QObject *parent) :
    QObject(parent)
{
}

LayerManager::LayerManager(QObject *parent, UIContextModel *context) : QObject(parent), _uicontext(context)
{
    IlwisTypes metatype = itCOLLECTION | itCATALOGVIEW;
    Resource res("Global Property Editors", metatype);
    _layers.append(new CoverageLayerModel(_layers.size(), res, _uicontext->propertyEditors(metatype),0, this));
}
void LayerManager::addVisualizationModel(CoverageLayerModel *newmodel)
{
    _layers.insert(1,newmodel);
}

void LayerManager::addDataSource(const QString &url, const QString &typeName, Ilwis::Geodrawer::DrawerInterface *drawer)
{
    IlwisTypes tp = IlwisObject::name2Type(typeName);
    if ( tp == itUNKNOWN)
        return;
    Resource resource = mastercatalog()->name2Resource(url,tp);
    if ( !resource.isValid())
        return;

    _layers.insert(1,new CoverageLayerModel(_layers.size(), resource, _uicontext->propertyEditors(tp), drawer, this));
}

bool LayerManager::zoomInMode() const
{
    return _zoomInMode;
}

void LayerManager::setZoomInMode(bool yesno)
{
    _zoomInMode = yesno;
}

bool LayerManager::hasSelectionDrawer() const
{
    return _hasSelectionDrawer;
}

void LayerManager::setHasSelectionDrawer(bool yesno)
{
    _hasSelectionDrawer = yesno;
}

QQmlListProperty<CoverageLayerModel> LayerManager::layers()
{
    return QQmlListProperty<CoverageLayerModel>(this, _layers);
}

CoverageLayerModel *LayerManager::layer(quint32 layerIndex){
    if ( layerIndex < _layers.size())
        return _layers[layerIndex];
    return 0;
}

void LayerManager::init()
{

}
