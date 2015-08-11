import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Controls.DropableItem{
    width : 250
    height : 0
    clip:true

    Rectangle {
        id : container
        height: parent.height
        width : parent.width
        border.width : 1
        border.color : Global.edgecolor
        radius: 5
        property var itemArray : ["aap","noot","mies"]

        Column {

            width : parent.width - 7
            height : 70
            y : 5
            spacing : 4
            x : 3

            EditorHeader{}

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
                ScrollView{
                    anchors.fill : parent
                    ListView {
                        anchors.fill : parent
                        model : container.itemArray
                        delegate: Component {
                            Text{
                                x : 4
                                text : modelData
                            }
                        }
                    }
                }
            }
            Button{
                anchors.right: parent.right
                anchors.rightMargin: 2
                width : 80
                height : 22
                text : "New Item"
                onClicked: {
                    newItem.enabled = true
                }
            }
            Text {
                width : parent.width
                height : 20
                text : "Thematic Item"
                opacity : newItem.enabled ? 1 : 0
            }

            Column {
                id :newItem
                width : parent.width
                height : 100
                enabled : false
                opacity : enabled ? 1.0 : 0
                spacing: 3
                Controls.TextEditLabelPair{
                    labelText: qsTr("Name")
                    labelWidth: 100
                    width : parent.width
                }

                Controls.TextEditLabelPair{
                    labelText: qsTr("Code")
                    labelWidth: 100
                    width : parent.width
                }

                Controls.TextEditLabelPair{
                    labelText: qsTr("Description")
                    labelWidth: 100
                    width : parent.width
                }
                Button{
                    anchors.right: parent.right
                    anchors.rightMargin: 2
                    width : 80
                    height : 22
                    text : "Add Item"
                    onClicked: {
                        newItem.enabled = false
                    }
                }
            }
        }
    }

}
