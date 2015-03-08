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
    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name

        if ( name === "")
            name = "redbuttonr.png"

         var iconP = "../../images/" + name
         return iconP
     }
    Layout.minimumHeight: 16

    color : Global.alternatecolor2
    Rectangle {
        id : header
        width : parent.width
        height : 18
        color : Global.alternatecolor4
        Text {
            text : "Display Options"
            font.bold: true
            font.pointSize: 8
            anchors.left : parent.left
            anchors.leftMargin: 5
        }
    }

    Item {
        id : firstColumn
        width : 220
        anchors.top : header.bottom
        height : propertyEditorBar.height - header.height - 18
        x : 5
        Text{
            id : layersLabel
            text : qsTr("Layers")
            font.weight: Font.DemiBold
        }
        Rectangle {
            width : parent.width
            anchors.top: layersLabel.bottom
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
                        editorsList.setSource("PropertyEditorsList.qml",{"editors" :coverage.propertyEditors })
                    }

                }

                id : layersList
                model : manager.layers
                anchors.fill: parent
                anchors.margins: 4
                delegate: LayerDelegate{}
                highlight: highlight
                focus: true

            }
        }

    }
    Button{
        id : deleteButton
        height : 14
        width: 80
        text: "remove layer"
        anchors.top : firstColumn.bottom
        x : 5
    }
    Item {
        id : editorsColumn
        width : 150
        height : propertyEditorBar.height - header.height - 18
        anchors.top : header.bottom
        anchors.left : firstColumn.right
        anchors.leftMargin: 10

        Text{
            id : editorsLabel
            text : qsTr("Option Editors")
            font.weight: Font.DemiBold
            opacity : editorsList.item ? 1 : 0
        }
        Loader {
            width : parent.width
            anchors.top: editorsLabel.bottom
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            id : editorsList
        }
    }

}





