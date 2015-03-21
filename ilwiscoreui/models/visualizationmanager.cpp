#include <QDebug>
#include <memory>
#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "iooptions.h"
#include "uicontextmodel.h"
#include "coveragelayermodel.h"
#include "coverage.h"
#include "visualizationmanager.h"

using namespace Ilwis;

LayerManager::LayerManager(QObject *parent) :
    QObject(parent)
{
}

LayerManager::LayerManager(QObject *parent, UIContextModel *context) : QObject(parent), _uicontext(context)
{
    IlwisTypes metatype = itCOLLECTION | itCATALOGVIEW;
    Resource res("Global Layer", metatype);
    CoverageLayerModel * model = new CoverageLayerModel(_layers.size(), res,0, this);
    model->iconPath("layers.png");
    _layers.append(model);
}
void LayerManager::addVisualizationModel(CoverageLayerModel *newmodel)
{
    _layers.insert(1,newmodel);
}

void LayerManager::addDataSource(const QUrl &url, IlwisTypes tp, Ilwis::Geodrawer::DrawerInterface *drawer)
{
    try{
        if ( tp == itUNKNOWN)
            return;
        Resource resource = mastercatalog()->name2Resource(url.toString(),tp);
        if ( !resource.isValid())
            return;
        //IIlwisObject obj(resource);
        auto layer = new CoverageLayerModel(_layers.size(), resource, drawer, this);
        if  ( _layers.size() == 1)
            _layers.push_back(layer);
        else
            _layers.insert(1,new CoverageLayerModel(_layers.size(), resource, drawer, this));
        emit layerChanged();
    }
    catch(const ErrorObject& ){
    }
    catch(const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
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

void LayerManager::layersView(LayersViewCommandInterface *view)
{
    _layersView = view;
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

QString LayerManager::layerInfo(const Coordinate &crd, const QString& attrName) const
{
    std::vector<QString> texts;
    for(CoverageLayerModel *layer : _layers){
        if ( layer->object().isValid() && hasType(layer->object()->ilwisType(), itCOVERAGE)){
            ICoverage cov = layer->object().as<Coverage>();
            QVariant value = cov->coord2value(crd,layer->activeAttribute());
            if ( value.isValid()){
                texts.push_back(value.toString());
            }
        }

    }
    QString outtext;
    for(auto txt : texts){
        if ( outtext.size() != 0)
            outtext += "; ";
        outtext += txt;
    }
    if ( outtext == "")
        outtext = "?";
    return outtext;
}

void LayerManager::init()
{

}
