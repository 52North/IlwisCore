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
        height: 560
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

         Item {
            width : parent.width
            height : 60
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            Button {
                id : applybutton
                anchors.right: parent.right
                anchors.rightMargin: 3
                anchors.bottom: parent.bottom
                width : 70
                text : qsTr("Apply")
                onClicked: {
                    dropItem.state = "invisible"
                    var itemstring = ""
                    if ( commonpart.domitems.item.model){

                        for(var i = 0; i < commonpart.domitems.item.model.count; ++i){
                            if (itemstring !== "")
                                itemstring += "|"
                            itemstring += commonpart.domitems.item.model.get(i).paletteColor;
                        }
                        var createInfo = {parentdomain : commonpart.parentdomain, type : "itemdomain", valuetype : "palette", name :  commonpart.name, items : itemstring, description : commonpart.description,strict : commonpart.strict}
                        var ilwisid = objectcreator.createObject(createInfo)
                    }
                }

            }
            Button {
                id : closebutton
                anchors.right: applybutton.left
                anchors.rightMargin: 5
                anchors.bottom: parent.bottom
                width : 70
                text : qsTr("Close")
                   onClicked: {
                    dropItem.state = "invisible"
                }
            }
        }

    }

}




