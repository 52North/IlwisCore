import QtQuick 2.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Item {
    id : displaypropertiesColumn

    property var displayEditorModel

    Rectangle {
        id : propertiesLabel
        width : 160
        height : 18
        color : uicontext.paleColor
        Text{
            text : qsTr("Visual properties")
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }
        Controls.ToolTip{
            target: propertiesLabel
            text : qsTr("List of editors for the visual properties of the selected attribute that can be changed")
        }

    }
    Rectangle {
        id : displayEditors
        color : uicontext.lightestColor
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

