import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Controls.DropableItem{
    id : dropItem
    width : 250
    height : 0

    clip:true

    function addDomainItems(items,clear){
        if ( clear)
            container.itemArray = []
        for(var i = 0; i < items.length; ++i){

            container.itemArray.append({paletteColor : items[i].paletteColor})
        }
        commonpart.domitems.item.model = container.itemArray
    }


    Rectangle {
        id : container
        height: 630
        width : parent.width
        border.width : 1
        border.color : Global.edgecolor
        radius: 5
        property var parentDomain
        property ListModel itemArray : ListModel{}

        ItemDomainCommonPart{
            id : commonpart
            domaintype: "itemdomain"
            valuetype: "Palette Color"
            parentItemList : "SelectColorItem.qml"
            newItemEditor: "AddNewColorItem.qml"
            domitems.source : "PaletteColorList.qml"
        }

        function apply(overwrite) {
            dropItem.state = "invisible"
            var itemstring = ""
            if ( commonpart.domitems.item.model){
                if (!overwrite){
                    if ( mastercatalog.exists("ilwis://internalcatalog/"+ commonpart.name, "itemdomain")){
                        return false;
                    }
                }

                for(var i = 0; i < commonpart.domitems.item.model.count; ++i){
                    if (itemstring !== "")
                        itemstring += ","
                    itemstring += commonpart.domitems.item.model.get(i).paletteColor;
                }
                itemstring = "'" + itemstring + "'"
                 var url = applyBut.currentCatalogCorrectUrl() + "/"+ commonpart.name
                var createInfo = {parentdomain : commonpart.parentdomain, type : "itemdomain", valuetype : "palette", name : url, items : itemstring, description : commonpart.description,strict : commonpart.strict}
                var ilwisid = objectcreator.createObject(createInfo)

                return true
            }
        }

         ApplyCreateButtons {
            width : parent.width
            height : 30
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            id : applyBut
            createObject: container.apply

        }

    }

}




