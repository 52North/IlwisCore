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
    fillAttributes();
}

void CoverageLayerModel::fillAttributes() {
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

void CoverageLayerModel::resetVisualAttributeModel(const QString& attributeName) {
    if ( object().isValid()){
        resetAttributeModel(attributeName);
        auto attrList = attributes();
        for(int i=0; attrList.count != 0  && i < attrList.count(&attrList); ++i){
            auto *attr = attrList.at(&attrList, i);
            if ( attr->columnDef().name() == attributeName){
                IlwisTypes valueType =  attr->columnDef().datadef().domain()->valueType();
                if ( hasType(valueType, itNUMBER|itDOMAINITEM|itSTRING|itCOLOR)){
                    // i+1, because the first element in the visualaatributes is the layerwide attribute which doesnt exist with the 'real' attributes
                    _visualAttributes[i+1] = new VisualAttributeModel(attr->columnDef(),this,object());
                    break;
                }
            }
        }
    }
}

Geodrawer::DrawerInterface *CoverageLayerModel::drawer()
{
    return _drawer;
}

const Geodrawer::DrawerInterface *CoverageLayerModel::drawer() const
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
    try{
        if ( index == _activeAttribute)
            return;

        _activeAttribute = index;
        if ( index < _visualAttributes.size() && _drawer){
            _drawer->setAttribute("activevisualattribute", _visualAttributes[index]->attributename());
            _drawer->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
            _drawer->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, Ilwis::IOOptions());
            _drawer->redraw();
            emit onActiveAttributeIndexChanged();
        }
    } catch(const ErrorObject& err){

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

bool CoverageLayerModel::showInfo() const
{
    return _showInfo;
}

void CoverageLayerModel::showInfo(bool yesno)
{
    _showInfo = yesno;
}

void CoverageLayerModel::updateAttribute(const QString& attrName, const QString& rprTxt)
{
    resetVisualAttributeModel(attrName);
    VisualAttributeModel *attrModel = visualAttribute(attrName);
    if ( attrModel){
        IDomain dom = attrModel->columnDef().datadef().domain();
        IRepresentation rpr = attrModel->representation()->copyWith(dom);
        rpr->colors( ColorLookUp::create(dom, rprTxt));
        attrModel->representation(rpr);

        drawer()->resetVisualProperty(attrName, rpr);
        emit visualAttributesChanged();
        _drawer->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
        _drawer->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, IOOptions());
        _drawer->redraw();
    }


}

void CoverageLayerModel::drawer(Geodrawer::DrawerInterface *drw)
{
    _drawer = drw;
}

void CoverageLayerModel::addVisualAttribute(VisualAttributeModel *attr)
{
    _visualAttributes.append(attr);
    emit visualAttributesChanged();
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




