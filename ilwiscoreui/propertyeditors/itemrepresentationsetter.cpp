#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "representation.h"
#include "representationelement.h"
#include "colorlookup.h"
#include "palettecolorlookup.h"
#include "models/visualattributemodel.h"
#include "itemrepresentationsetter.h"

REGISTER_PROPERTYEDITOR("itemrepresentationsetter",ItemRepresentationSetter)

ItemRepresentationSetter::ItemRepresentationSetter(QObject *parent) :
    VisualAttributeEditor("itemrepresentationsetter",TR("Legend"),QUrl("ItemRepresentation.qml"), parent)
{

}

void ItemRepresentationSetter::prepare(VisualAttributeModel *vattrib, const IIlwisObject &obj, const ColumnDefinition &coldef)
{
    VisualAttributeEditor::prepare( vattrib, obj, coldef);

    if ( attribute()->layer() && attribute()->layer()->drawer()){
        IlwisData<ItemDomain<DomainItem>> itemdom = coldef.datadef().domain().as<ItemDomain<DomainItem>>();
        for(auto item : itemdom){
            if (attribute()->representation().isValid()){
                QColor clr = attribute()->representation()->colors()->value2color(item->raw());
                _rprElements.push_back(new RepresentationElement(item->raw(), item->name(), clr,this));
            }

        }

        emit rprNameChanged();
    }
}

bool ItemRepresentationSetter::canUse(const IIlwisObject &obj, const ColumnDefinition &coldef) const{
    if ( coldef.isValid()){
        if(hasType(coldef.datadef().domain()->ilwisType(), itITEMDOMAIN)){
            if ( hasType(coldef.datadef().domain()->valueType(), itINDEXEDITEM | itNAMEDITEM))
                 return false;
            return true;
        }
    }
    return false;
}

VisualAttributeEditor *ItemRepresentationSetter::create()
{
    return new ItemRepresentationSetter();
}

QQmlListProperty<RepresentationElement> ItemRepresentationSetter::representationElements()
{
    return  QQmlListProperty<RepresentationElement>(this, _rprElements);
}

void ItemRepresentationSetter::attributesChanged(Raw index, const QVariantMap& values){

    if(values.contains("color")) {
        QColor clr = values["color"].value<QColor>()  ;
        attribute()->representation()->colors()->setColor(index,clr);
        VisualAttributeEditor::attributesChanged(index, values);
    }
}

void ItemRepresentationSetter::representationChanged(const IRepresentation& rpr){
    if ( rpr->domain().isValid() && rpr->domain()->ilwisType() == itITEMDOMAIN){
        IlwisData<ItemDomain<DomainItem>> itemdom = rpr->domain().as<ItemDomain<DomainItem>>();
        _rprElements.clear();
        for(auto item : itemdom){
            QColor clr = rpr->colors()->value2color(item->raw());
            _rprElements.push_back(new RepresentationElement(item->raw(), item->name(), clr,this));

        }

        emit rprNameChanged();
    }
}

