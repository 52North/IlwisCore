#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "uicontextmodel.h"
#include "layermanager.h"
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
        for(int i=0; attrList.count != 0  && i < attrList.count(&attrList); ++i){
            auto *attr = attrList.at(&attrList, i);
            IlwisTypes valueType =  attr->columnDef().datadef().domain()->valueType();
            if ( hasType(valueType, itNUMBER|itDOMAINITEM|itSTRING|itCOLOR)){
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

int CoverageLayerModel::getActiveAttributeIndex() const
{
    return _activeAttribute;
}

void CoverageLayerModel::setActiveAttributeIndex(int index)
{
    if ( index == _activeAttribute)
        return;

    _activeAttribute = index;
    if ( index < _visualAttributes.size()){
        _drawer->setAttribute("activevisualattribute", _visualAttributes[index]->attributename());
        _drawer->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
        _drawer->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, Ilwis::IOOptions());
        _drawer->redraw();
        emit onActiveAttributeIndexChanged();
    }
}

QString CoverageLayerModel::activeAttribute() const
{
    if ( _activeAttribute < _visualAttributes.size()){
       return _visualAttributes[_activeAttribute]->attributename();
    }
    return sUNDEF;
}

QString CoverageLayerModel::visualAttributeByIndex(int index) const
{
    if ( index < _visualAttributes.size()){
       return _visualAttributes[index]->attributename();
    }
    return sUNDEF;
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




