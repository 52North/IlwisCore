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
        domainitems.model = container.itemArray
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

        Column {

            width : parent.width - 7
            height : parent.height
            y : 5
            spacing : 4
            x : 3

            EditorHeader{}

            Item {
                id : parentdom
                width : parent.width
                height : 20
                Text{
                    id : parentLabel
                    width : 100
                    height : 20
                    text : qsTr("Parent domain")
                }
                function isThematicDomain(objid){
                    var tp = mastercatalog.id2type(objid)
                    container.parentDomain = mastercatalog.id2object(objid, parentdom)
                    if ( container.parentDomain && tp === "itemdomain"){
                        return container.parentDomain.valuetype === "Named Identifier"
                    }else{
                        container.parentDomain = null
                        parentdomtxt.content = ""

                    }
                    return false
                }

                Controls.TextFieldDropArea{
                    id : parentdomtxt
                    anchors.left : parentLabel.right
                    anchors.right: parent.right
                    anchors.rightMargin: 4
                    height: 20

                    canUse: parentdom.isThematicDomain
                    readOnly: false
                    asName: false

                    onContentChanged: {
                        if ( content != ""){
                            itemloader.source = ""
                            itemloader.source = "SelectIdentifierItem.qml"
                        }
                        else
                            dropItem.state = "invisible"
                    }
                }
            }
            Controls.TextEditLabelPair{
                id : namevalue
                labelText: qsTr("Name")
                labelWidth: 100
                width : parent.width
            }
            Controls.TextAreaLabelPair{
                id : descvalue
                labelText: qsTr("Description")
                width : parent.width
                height : 40
                labelWidth: 100
            }
            CheckBox{
                id : cbstrict
                text: qsTr("Strict")
                checked: true
                enabled : parentdomtxt.content != ""
                style : Base.CheckBoxStyle1{}
            }
            Text {
                text : qsTr("Current items")
                font.bold: true
            }
            Rectangle {
                id : itemList
                width : parent.width
                height : 120
                border.width: 1
                border.color : Global.edgecolor
                radius : 3

                ItemList {
                    id : domainitems
                    width : parent.width
                    height : parent.height
                }

            }
            Loader {
                id : itemloader
                width : parent.width
                source : "AddNewIdentifierItem.qml"
                height : 170
                onSourceChanged: {
                    if ( item && container.parentDomain !== undefined){
                        item.itemdomain = container.parentDomain
                    }
                }
            }
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
                    if ( domainitems.model){
                        for(var i = 0; i < domainitems.model.length; ++i){
                            if (itemstring !== "")
                                itemstring += "|"
                            itemstring += domainitems.model[i].name;
                        }

                        var createInfo = {parentdomain : parentdomtxt.content, type : "itemdomain", valuetype : "identifier", name :  namevalue.content, items : itemstring, description : descvalue.content,strict : cbstrict.checked}
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


