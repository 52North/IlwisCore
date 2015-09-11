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

    function addDomainItems(items){
        console.debug(items.length)
        for(var i = 0; i < items.length; ++i){
            container.itemArray.push(items[i])
        }
        domainitems.model = container.itemArray
    }


    Rectangle {
        id : container
        height: parent.height
        width : parent.width
        border.width : 1
        border.color : Global.edgecolor
        radius: 5
        property var parentDomain
        property var itemArray : []

        Column {

            width : parent.width - 7
            height : 440
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
                        return container.parentDomain.valuetype === "Thematic class"
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
                            itemloader.source = "SelectThematicItem.qml"
                        }
                        else
                            dropItem.state = "invisible"
                    }
                }
            }
            Controls.TextEditLabelPair{
                labelText: qsTr("Name")
                labelWidth: 100
                width : parent.width
            }
            Controls.TextAreaLabelPair{
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
                height : 110
                border.width: 1
                border.color : Global.edgecolor
                radius : 3
                ScrollView{
                    anchors.fill : parent
                    ListView {
                        id : domainitems
                        anchors.fill : parent
                        delegate: Component {
                            Text{
                                x : 4
                                text : modelData
                            }
                        }
                    }
                }
            }
            Loader {
                id : itemloader
                width : parent.width
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
            height : 30
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            Button {
                id : applybutton
                anchors.right: parent.right
                anchors.rightMargin: 3
                width : 70
                text : qsTr("Apply")
                y : 10
                onClicked: {
                    dropItem.state = "invisible"
                    var itemstring
                    for(var i = 0; i < domainitems.model; ++i){
                        if (itemstring !== null)
                            itemstring += "|"
                        itemstring += domainitems.model[i];
                    }

                    var createInfo = {parentdomain : parentdomtxt.content, type : "itemdomain", valuetype : "thematic", name :  namevalue.content, items : itemstring, description : descvalue.content,strict : cbstrict.checked}
                    var ilwisid = objectcreator.createObject(createInfo)
                }

            }
            Button {
                id : closebutton
                anchors.right: applybutton.left
                anchors.rightMargin: 5
                width : 70
                text : qsTr("Close")
                y : 10
                onClicked: {
                    dropItem.state = "invisible"
                }
            }
        }

    }

}
