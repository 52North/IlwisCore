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

VisualizationManager::VisualizationManager(QObject *parent) :
    QObject(parent)
{
}

VisualizationManager::VisualizationManager(QObject *parent, UIContextModel *context) : QObject(parent), _uicontext(context)
{
    IlwisTypes metatype = itCOLLECTION | itCATALOGVIEW;
    Resource res("Global Property Editors", metatype);
    _layers.append(new CoverageLayerModel(_layers.size(), res, _uicontext->propertyEditors(metatype),0, this));
}
void VisualizationManager::addVisualizationModel(CoverageLayerModel *newmodel)
{
    _layers.insert(1,newmodel);
}

void VisualizationManager::addDataSource(const QString &url, const QString &typeName, Ilwis::Geodrawer::DrawerInterface *drawer)
{
    IlwisTypes tp = IlwisObject::name2Type(typeName);
    if ( tp == itUNKNOWN)
        return;
    Resource resource = mastercatalog()->name2Resource(url,tp);
    if ( !resource.isValid())
        return;

    _layers.insert(1,new CoverageLayerModel(_layers.size(), resource, _uicontext->propertyEditors(tp), drawer, this));
}

bool VisualizationManager::zoomInMode() const
{
    return _zoomInMode;
}

void VisualizationManager::setZoomInMode(bool yesno)
{
    _zoomInMode = yesno;
}

bool VisualizationManager::hasSelectionDrawer() const
{
    return _hasSelectionDrawer;
}

void VisualizationManager::setHasSelectionDrawer(bool yesno)
{
    _hasSelectionDrawer = yesno;
}

QQmlListProperty<CoverageLayerModel> VisualizationManager::layers()
{
    return QQmlListProperty<CoverageLayerModel>(this, _layers);
}

CoverageLayerModel *VisualizationManager::layer(quint32 layerIndex){
    if ( layerIndex < _layers.size())
        return _layers[layerIndex];
    return 0;
}

void VisualizationManager::init()
{

}
