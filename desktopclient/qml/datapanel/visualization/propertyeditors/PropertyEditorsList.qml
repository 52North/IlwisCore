import QtQuick 2.0
import "../../Global.js" as Global

Item {
    property var editors
    function getAttributeIndex() { return editorList.currentIndex}
    property int attributeIndex : 0
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
                        x : 5
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                editorList.currentIndex = index
                                propertyEditorBar.setEditor(index)
                            }
                        }
                    }
                }
            }
        }
         highlight: highlight2
         model : editors
         currentIndex: attributeIndex
    }
}


