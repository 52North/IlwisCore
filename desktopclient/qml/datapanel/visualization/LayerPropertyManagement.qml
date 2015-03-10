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

    function setEditor(propindex){
        var coverage = manager.layer(layersList.currentIndex)
        var editor = coverage.propertyEditors[propindex]
        propertyEditor.setSource(editor.qmlUrl,{"editor" : editor})
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
                        editorsListLoader.setSource("PropertyEditorsList.qml",{"editors" :coverage.propertyEditors })
                    }

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

        Text{
            id : propertiesLabel
            text : qsTr("Display Properties")
            font.weight: Font.DemiBold
        }
        Rectangle {
            color : Global.alternatecolor2
            border.color: "lightgrey"
            border.width: 1
            width : parent.width
            anchors.top: propertiesLabel.bottom
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

        Text{
            id : editorsLabel
            text : qsTr("Property Editor")
            font.weight: Font.DemiBold
        }

        Rectangle {
            color : Global.alternatecolor2
            border.color: "lightgrey"
            border.width: 1
            width : parent.width
            anchors.top: editorsLabel.bottom
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            Loader {
                id : propertyEditor
                anchors.fill : parent

            }
        }
    }

}





