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
            if ( items[i].name === "")
                continue;
            var duplicate = false
            for ( var j = 0; j < container.itemArray.length; ++j){
                // double names allowed
                if( items[i].name === container.itemArray[j].name){
                    duplicate = true
                }
                if ( duplicate)
                    break
            }
            if ( !duplicate)
                container.itemArray.push(items[i])
        }
        commonpart.domitems.item.model = container.itemArray
    }


    Rectangle {
        id : container
        height: 520
        width : parent.width
        border.width : 1
        border.color : Global.edgecolor
        radius: 5
        property var parentDomain
        property var itemArray : []

        ItemDomainCommonPart{
            id : commonpart
            domaintype: "itemdomain"
            valuetype: "Named Identifier"
            parentItemList : "SelectIdentifierItem.qml"
            newItemEditor: "AddNewIdentifierItem.qml"
            domitems.source : "ItemList.qml"
        }

        function apply(overwrite) {
            var itemstring = ""
            if ( commonpart.domitems.item.model){
                if (!overwrite){
                    if ( mastercatalog.exists("ilwis://internalcatalog/"+ commonpart.name, "domain")){
                        return false;
                    }
                }
                for(var i = 0; i < commonpart.domitems.item.model.length; ++i){
                    if (itemstring !== "")
                        itemstring += "|"
                    itemstring += commonpart.domitems.item.model[i].name;
                }
               var url = applyButton.currentCatalogCorrectUrl() + "/"+ commonpart.name
                var createInfo = {parentdomain : commonpart.parentdomain, type : "itemdomain", valuetype : "identifier", name :  url, items : itemstring, description : commonpart.description,strict : commonpart.strict}
                objectcreator.createObject(createInfo)

                return true
            }
        }
        ApplyCreateButtons {
            width : parent.width
            height : 60
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            id : applyButton
            createObject: container.apply
         }

    }

}


