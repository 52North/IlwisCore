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
        if ( editor){
            var res = editor.isAttributeDependent;
            if ( res)
                attributesList.model = editor.attributes

            if ( editorColumn2.state == "minimized"){
                editorColumn1.state = "minimized"
                editorColumn2.state = "maximized"
                propertyEditor2.setSource(editor.qmlUrl,{"editor" : editor})

            }else {
                editorColumn2.state = "minimized"
                editorColumn1.state = "maximized"
                propertyEditor1.setSource(editor.qmlUrl,{"editor" : editor})
            }
            if ( editor.isAttributeDependent){
                attributeListColumn.state = "maximized"
            }else{
               attributeListColumn.state = "minimized"
            }
        }
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
        width : 180
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
        width : 130
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
                id : attributesLabel
                text : qsTr("Attributes")
                font.weight: Font.DemiBold
                anchors.verticalCenter: parent.verticalCenter
                width : 0
                opacity: 0
            }
            Text{
                text : qsTr("Property Editor")
                font.weight: Font.DemiBold
                anchors.verticalCenter: parent.verticalCenter
                anchors.left : attributesLabel.right
            }
        }

        Rectangle {
            id : attributeListColumn
            color : Global.alternatecolor2
            border.color: "lightgrey"
            border.width: 1
            width : 0
            anchors.top: editorsLabel.bottom
            anchors.topMargin: 2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5

            ListView {

                id : attributesList
                Component {
                    id: attributeHighlight

                    Rectangle {
                        width: attributesList.width; height: 18
                        color: Global.selectedColor; radius: 2
                        y: (attributesList && attributesList.currentItem) ? attributesList.currentItem.y : 0
                        Behavior on y {
                            SpringAnimation {
                                spring: 3
                                damping: 0.2
                            }
                        }
                    }
                }
                anchors.fill: parent
                highlight: attributeHighlight
                delegate: Component {
                    Loader {
                        sourceComponent: Component {
                            Text {
                                x : 4
                                text: modelData
                                width : 100
                                height : Global.rowHeight
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: {
                                        attributesList.currentIndex = index
                                    }
                                }
                            }
                        }
                    }
                }
            }

            states: [
                State { name: "maximized"
                    PropertyChanges { target: attributeListColumn; width : 100 }
                    PropertyChanges { target: attributesLabel; width : 100 }
                    PropertyChanges { target: attributesLabel; opacity : 1 }
                },
                State {
                    name : "minimized"
                    PropertyChanges {target: attributeListColumn; width : 0}
                    PropertyChanges { target: attributesLabel; width : 0 }
                    PropertyChanges { target: attributesLabel; opacity : 0 }
                }

            ]
            transitions: [
                Transition {
                    NumberAnimation { properties: "width"; duration : 500 ; easing.type: Easing.InOutCubic }
                    NumberAnimation { properties: "opacity"; duration : 500 ; easing.type: Easing.InOutCubic }
                }
            ]
        }

        Rectangle {
            id : editorColumn1
            color : Global.alternatecolor2
            border.color: "lightgrey"
            border.width: 1
            anchors.right: parent.right
            anchors.top: editorsLabel.bottom
            anchors.topMargin: 2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.left: attributeListColumn.right
            anchors.leftMargin: 3
            state : "maximized"
            Loader {
                id : propertyEditor1
                anchors.fill : parent


            }
            states: [
                State { name: "maximized"
                        PropertyChanges { target: editorColumn1; opacity : 1 }
                },
                State {
                    name : "minimized"
                        PropertyChanges { target: editorColumn1; opacity : 0 }
                }

            ]
            transitions: [
                Transition {
                    NumberAnimation { properties: "opacity"; duration : 500 ; easing.type: Easing.InOutCubic }
                }
            ]

        }

        Rectangle {
            id : editorColumn2
            color : Global.alternatecolor2
            border.color: "lightgrey"
            border.width: 1
            anchors.right: parent.right
            anchors.top: editorsLabel.bottom
            anchors.topMargin: 2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.left: attributeListColumn.right
            anchors.leftMargin: 3
            Loader {
                id : propertyEditor2
                anchors.fill : parent


            }
            states: [
                State { name: "maximized"
                        PropertyChanges { target: editorColumn2; opacity : 1 }
                },
                State {
                    name : "minimized"
                        PropertyChanges { target: editorColumn2; opacity : 0 }
                }

            ]
            transitions: [
                Transition {
                    NumberAnimation { properties: "opacity"; duration : 500 ; easing.type: Easing.InOutCubic }
                }
            ]

        }
    }

}





