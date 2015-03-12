import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Rectangle {
    id : propertyEditorBar

    property var renderer
    property int layerIndex : 0

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name

        if ( name === "")
            name = "redbuttonr.png"

         var iconP = "../../images/" + name
         return iconP
     }

    function setEditor(propindex){
        var coverage = manager.layer(layersList.currentIndex)
        var editor = coverage.propertyEditors[propindex]
        if ( editor)
            propertyEditor.setSource(editor.qmlUrl,{"editor" : editor})
    }

    Layout.minimumHeight: 16

    color : Global.alternatecolor2
    Rectangle {
        id : header
        width : parent.width
        height : 18
        color : Global.headerdark
        Text {
            text : "Display Options"
            font.bold: true
            font.pointSize: 8
            anchors.left : parent.left
            anchors.leftMargin: 5
            color : "white"
        }
    }

    Item {
        id : firstColumn
        width : 220
        anchors.top : header.bottom
        height : propertyEditorBar.height - header.height - 18
        x : 5
        Rectangle {
            id : layersLabel
            width : parent.width + 10
            height : 18
            color : Global.headerlight
            Text{
                text : qsTr("Layers")
                font.weight: Font.DemiBold
                x : 5
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Rectangle {
            width : parent.width
            anchors.top: layersLabel.bottom
            anchors.topMargin: 2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            color : Global.alternatecolor2
            border.color: "lightgrey"
            border.width: 1

            Component {
                id: highlight

                Rectangle {
                    width: layersList.width; height: 18
                    color: Global.selectedColor; radius: 2
                    y: (layersList && layersList.currentItem) ? layersList.currentItem.y : 0
                    Behavior on y {
                        SpringAnimation {
                            spring: 3
                            damping: 0.2
                        }
                    }
                }
            }
            ListView {
                function setEditors(){
                    var coverage = manager.layer(currentIndex)
                    if ( coverage){
                        var attributeIndex = 0
                        if ( editorsListLoader.item){
                            attributeIndex = editorsListLoader.item.getAttributeIndex()
                        }

                        editorsListLoader.setSource("PropertyEditorsList.qml",{"editors" :coverage.propertyEditors })
                        editorsListLoader.item.attributeIndex = attributeIndex
                        setEditor(attributeIndex)
                    }

                }
                onCountChanged: {
                    currentIndex = layerIndex + 1 ; // the new layer will be on top, so the last index has shifted one up
                    setEditors()
                }

                id : layersList
                model : manager.layers
                anchors.fill: parent
                anchors.margins: 4
                delegate: LayerDelegate{}
                highlight: highlight
                focus: true
                clip : true

            }
        }

    }
    Button{
        id : deleteButton
        height : 14
        width: firstColumn.width
        opacity : layersList.height > 5 ? 14 : 0
        enabled : layersList.height > 5
        text: "remove layer"
        anchors.top : firstColumn.bottom
        x : 5
    }
    Item {
        id : displaypropertiesColumn
        width : 150
        height : propertyEditorBar.height - header.height - 18
        anchors.top : header.bottom
        anchors.left : firstColumn.right
        anchors.leftMargin: 10

        Rectangle {
            id : propertiesLabel
            width : 160
            height : 18
            color : Global.headerlight
            Text{
                text : qsTr("Display Properties")
                font.weight: Font.DemiBold
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Rectangle {
            color : Global.alternatecolor2
            border.color: "lightgrey"
            border.width: 1
            width : parent.width
            anchors.top: propertiesLabel.bottom
            anchors.topMargin: 2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            Loader {
                anchors.fill : parent
                id : editorsListLoader
            }
        }
    }
    Item {
        id : displayEditorColumn
        anchors.right: parent.right
        anchors.rightMargin: 10
        height : propertyEditorBar.height - header.height - 18
        anchors.top : header.bottom
        anchors.left : displaypropertiesColumn.right
        anchors.leftMargin: 10

        Rectangle {
            id : editorsLabel
            width : parent.width
            height : 18
            color : Global.headerlight
            Text{
                text : qsTr("Property Editor")
                font.weight: Font.DemiBold
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            color : Global.alternatecolor2
            border.color: "lightgrey"
            border.width: 1
            width : parent.width
            anchors.top: editorsLabel.bottom
            anchors.topMargin: 2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            Loader {
                id : propertyEditor
                anchors.fill : parent

            }
        }
    }

}





