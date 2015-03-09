import QtQuick 2.0
import "../../Global.js" as Global

Rectangle {

    color : Global.alternatecolor2
    border.color: "lightgrey"
    border.width: 1
    property var editors
    height : 18

    Component {
        id: highlight2

        Rectangle {
            width: editorList.width; height: 18
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
        id : editorList
        anchors.fill: parent
        clip : true
        delegate: Component {
            Loader {
                sourceComponent: Component {
                    Text {
                        text: displayName
                        width : 100
                        height : 18
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                editorList.currentIndex = index
                            }
                        }
                    }
                }
            }
        }
         highlight: highlight2
         model : editors
    }
}


