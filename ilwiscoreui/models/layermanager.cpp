#include <QDebug>
#include <memory>
#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "iooptions.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "geometries.h"
#include "georeference.h"
#include "uicontextmodel.h"
#include "coveragelayermodel.h"
#include "raster.h"
#include "layerinfoitem.h"
#include "layermanager.h"
#include "globallayermodel.h"

using namespace Ilwis;

LayerManager::LayerManager(QObject *parent) :
    QObject(parent)
{
}

LayerManager::LayerManager(QObject *parent, UIContextModel *context) : QObject(parent), _uicontext(context)
{
    _layers.append(new GlobalLayerModel(this));
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
        ICoverage coverage(resource);
        if ( _layers.size() > 0){

            if ( !coverage.isValid())
                return;
            if ( coverage->coordinateSystem()->isUnknown()) {// after the first layer, nothing with unknown can be added
                QString mes = QString("coordinate system 'unknown' not compatible with coordinate system of the layerview");
                kernel()->issues()->log(mes, IssueObject::itWarning);
                return;
            }
        }

        if ( _masterCsy == 0) {// first real layer sets the csy
            _masterCsy = new ResourceModel(coverage->coordinateSystem()->source(), this);
            _viewEnvelope = coverage->envelope();
            emit coordinateSystemChanged();
        }
        auto layer = new CoverageLayerModel(_layers.size(), resource, drawer, this);
        if  ( _layers.size() == 1)
            _layers.push_back(layer);
        else
            _layers.insert(1, layer);
        emit layerChanged();
        emit latlonEnvelopeChanged();
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

void LayerManager::setScreenGeoReference(const IGeoReference &grf)
{
    _screenGrf = grf;
}

void LayerManager::moveLayer(int index, LayerManager::LayerMovement type)
{
    if ( type == lmDOWN){
        if ( index + 1 < _layers.size())
            _layers.swap(index + 1, index + 2); // layer 0 is a placeholder layer, so + 1
    }else if ( type == lmUP ){
        if ( index > 0  && index + 1 < _layers.size())
            _layers.swap(index + 1, index);
    } else if ( type == lmREMOVE){
        if ( index >= 0 && index + 1 < _layers.size())
        _layers.removeAt(index + 1);
    }
}

IGeoReference LayerManager::screenGrf() const
{
    return _screenGrf;
}

void LayerManager::setLayerListName(const QString name)
{
    _layerListName = name;
}

QString LayerManager::layerListName() const
{
    return _layerListName;
}

ResourceModel *LayerManager::coordinateSystem() const
{
    return _masterCsy;
}

Ilwis::Geodrawer::DrawerInterface *LayerManager::rootDrawer()
{
    if ( _layers.size() > 1){
        return (Geodrawer::DrawerInterface *)(_layers[1]->drawer()->rootDrawer());
    }
    return 0;
}

const Geodrawer::DrawerInterface *LayerManager::rootDrawer() const
{
    if ( _layers.size() > 1){
        return (Geodrawer::DrawerInterface *)(_layers[1]->drawer()->rootDrawer());
    }
    return 0;
}

void LayerManager::layersView(LayersViewCommandInterface *view)
{
    _layersView = view;
    if ( _layers.size() > 0)
        _layers[0]->drawer(view->rootDrawer());
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

QString LayerManager::layerInfo(const Coordinate &crdIn, const QString& attrName)
{
    if ( _zoomInMode) // when zooming we dont don' give info. costs too much performance
        return "";

    std::vector<QString> texts;


    _layerInfoItems.clear();
    for(CoverageLayerModel *layer : _layers){
        if ( layer->object().isValid() && hasType(layer->object()->ilwisType(), itCOVERAGE)){
            ICoverage cov = layer->object().as<Coverage>();
            Coordinate crd = crdIn;
            QVariant varCsy = layer->drawer()->attributeOfDrawer("rootdrawer","coordinatesystem");
            if ( varCsy.isValid()){
                ICoordinateSystem csy = varCsy.value<ICoordinateSystem>();
                if ( csy != cov->coordinateSystem()){
                    crd = cov->coordinateSystem()->coord2coord(csy, crd);
                }
            }

            auto item = new LayerInfoItem("Layer",cov->name(), this);
            item->icon(layer->iconPath());
            item->layerHeader(true);
            _layerInfoItems.push_back(item);
            if ( cov->ilwisType() == itRASTER){
                IRasterCoverage raster = cov.as<RasterCoverage>();
                Pixel pix = raster->georeference()->coord2Pixel(crd);
                QString pixtxt = QString("%1 %2").arg(pix.x + 1).arg(pix.y + 1);
                _layerInfoItems.push_back(new LayerInfoItem(TR("Pixel"), pixtxt, this));

            }
            QVariant value = cov->coord2value(crd,attrName);
            if ( value.isValid()){
                QString txt;
                if ( attrName != "")
                    txt = layer->value2string(value,attrName);
                else {
                    QVariantMap vmap = value.value<QVariantMap>();
                    int activeAttIndex = layer->getActiveAttributeIndex();
                    if ( activeAttIndex == 0){ // this is the pseudo attribute representing the whole layer, not wanted here
                        activeAttIndex = 1;
                    }
                    QVariant activeAttr =  vmap[layer->visualAttributeByIndex(activeAttIndex)];
                    txt = layer->value2string(activeAttr, layer->visualAttributeByIndex(activeAttIndex));
                    auto end = vmap.end();
                    for(auto item = vmap.begin(); item != end; ++item){
                        QString attTxt = layer->value2string(item.value(), item.key());
                        _layerInfoItems.push_back(new LayerInfoItem(item.key(), attTxt, this));
                    }
                    emit layerInfoItemsChanged();
                }
                if ( layer->showInfo())
                    texts.push_back(txt);

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

void LayerManager::refresh()
{
    emit layerChanged();
}

void LayerManager::init()
{

}

QVariantMap LayerManager::viewEnvelope() const
{
    QVariantMap vmap;
    vmap["minx"] = _viewEnvelope.min_corner().x;
    vmap["miny"] = _viewEnvelope.min_corner().y;
    vmap["maxx"] = _viewEnvelope.max_corner().x;
    vmap["maxy"] = _viewEnvelope.max_corner().y;

    return vmap;
}

QVariantMap LayerManager::latlonEnvelope() const
{
    if ( _layers.size() > 1){ // layer one is a bogus layer
        QVariant var = _layers[1]->drawer()->attributeOfDrawer("rootdrawer", "latlonenvelope");
        Envelope env = var.value<Envelope>();
        if ( env.isValid()){
            QVariantMap vmap;
            vmap["minx"] = env.min_corner().x;
            vmap["miny"] = env.min_corner().y;
            vmap["maxx"] = env.max_corner().x;
            vmap["maxy"] = env.max_corner().y;
            return vmap;
        }
    }
    return QVariantMap();
}


QQmlListProperty<LayerInfoItem> LayerManager::layerInfoItems()
{
    return QQmlListProperty<LayerInfoItem>(this,_layerInfoItems);
}
