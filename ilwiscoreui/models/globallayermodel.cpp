#include "kernel.h"
#include "visualattributemodel.h"
#include "globallayermodel.h"

GlobalLayerModel::GlobalLayerModel()
{

}

GlobalLayerModel::GlobalLayerModel(LayerManager *parent) :
    CoverageLayerModel(0,Ilwis::Resource("Global Layer", itCOLLECTION | itCATALOGVIEW),0, parent),
    _layermanager(parent)
{
    iconPath("layers.png");
    addVisualAttribute( new GlobalAttributeModel(TR("Grid"), "gridpropertyeditor", this));
    addVisualAttribute( new GlobalAttributeModel(TR("Geometry"), "", this));
    addVisualAttribute( new GlobalAttributeModel(TR("Background"), "backgroundlayereditor", this));
    addVisualAttribute( new GlobalAttributeModel(TR("3D"), "", this));
}

const Geodrawer::DrawerInterface *GlobalLayerModel::drawer() const
{
    if ( _layermanager)
        return _layermanager->rootDrawer();
    return 0;
}

Geodrawer::DrawerInterface *GlobalLayerModel::drawer()
{
    if ( _layermanager)
        return _layermanager->rootDrawer();
    return 0;
}



