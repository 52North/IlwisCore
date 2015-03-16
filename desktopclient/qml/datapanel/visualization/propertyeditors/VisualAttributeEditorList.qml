import QtQuick 2.0
import "../../../Global.js" as Global

Item {
    id : displaypropertiesColumn
    width : 130
    property var currentVisualAttribute

    onCurrentVisualAttributeChanged: {
        editorList.model =  currentVisualAttribute.propertyEditors
    }

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
        id : displayEditors
        color : Global.alternatecolor2
        border.color: "lightgrey"
        border.width: 1
        width : parent.width
        anchors.top: propertiesLabel.bottom
        anchors.topMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        ListView{
            id : editorList
            anchors.fill: parent
            Component {
                id: editorHighlight

                Rectangle {
                    width: editorList.width - 6; height: 14
                    x : 3
                    color: Global.selectedColor; radius: 2
                    y: (editorList && editorList.currentItem) ? editorList.currentItem.y : 0
                    Behavior on y {
                        SpringAnimation {
                            spring: 3
                            damping: 0.2
                        }
                    }
                }
            }
            highlight: editorHighlight
            delegate: Component {
                Loader {
                    sourceComponent: Component {
                        Text {
                            x : 4
                            text: editorName
                            width : displayEditors.width
                            height : 14
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    editorColumn.currentEditor = editorList.model[index]
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

