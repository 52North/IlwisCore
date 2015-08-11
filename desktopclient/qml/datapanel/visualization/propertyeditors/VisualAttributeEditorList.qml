import QtQuick 2.0
import "../../../Global.js" as Global

Item {
    id : displaypropertiesColumn

    property var displayEditorModel

    Rectangle {
        id : propertiesLabel
        width : 160
        height : 18
        color : Global.alternatecolor3
        Text{
            text : qsTr("Visual properties")
            font.weight: Font.DemiBold
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    Rectangle {
        id : displayEditors
        color : Global.alternatecolor2
        width : parent.width
        anchors.top: propertiesLabel.bottom
        anchors.topMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        ListView{
            id : editorList
            anchors.fill: parent
            model : displayEditorModel
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
                            text: displayName
                            width : displayEditors.width
                            height : 14
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    editorList.currentIndex = index
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

