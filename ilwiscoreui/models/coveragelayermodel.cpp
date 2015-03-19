#include "kernel.h"
#include "uicontextmodel.h"
#include "visualizationmanager.h"
#include "coveragelayermodel.h"
#include "visualattributemodel.h"


CoverageLayerModel::CoverageLayerModel()
{
}

CoverageLayerModel::CoverageLayerModel(quint32 layerInd, const Ilwis::Resource &resource,
                                       Ilwis::Geodrawer::DrawerInterface *drawer, QObject *obj) :
    IlwisObjectModel(resource, obj), _drawer(drawer), _layerIndex(layerInd)
{
    if ( object().isValid()){
        _visualAttributes.push_back(new LayerAttributeModel(this,object()));
        auto attrList = attributes();
        for(int i=0; i < attrList.count(&attrList); ++i){
            auto *attr = attrList.at(&attrList, i);
            IlwisTypes valueType =  attr->columnDef().datadef().domain()->valueType();
            if ( hasType(valueType, itNUMBER|itDOMAINITEM)){
                _visualAttributes.push_back(new VisualAttributeModel(attr->columnDef(),this,object()));
            }
        }
    }
}

Geodrawer::DrawerInterface *CoverageLayerModel::drawer()
{
    return _drawer;
}

bool CoverageLayerModel::active() const
{
    if ( _drawer){
        return _drawer->isActive();
    }
    return false;
}

void CoverageLayerModel::setActive(bool yesno)
{
    if ( _drawer){
        _drawer->active(yesno);
        _drawer->redraw();
        emit onActiveChanged();
    }
}

quint32 CoverageLayerModel::layerIndex() const
{
    return _layerIndex;
}

VisualAttributeModel *CoverageLayerModel::visualAttribute(const QString &name)
{
    for(auto attr : _visualAttributes){
        if ( attr->attributename() == name)
            return attr;
    }
    return 0;
}

QQmlListProperty<VisualAttributeModel> CoverageLayerModel::visualAttributes()
{
    return QQmlListProperty<VisualAttributeModel>(this, _visualAttributes) ;
}




