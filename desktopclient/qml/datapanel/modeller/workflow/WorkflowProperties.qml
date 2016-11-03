import QtQuick 2.0
import "../../../Global.js" as Global

Column {
    property alias currentIndex : layerProperties.currentIndex
    property alias propertyItems : layerProperties.model
    Text {
        id : label
        text : qsTr("Workflow Forms")
        font.bold: true
        height : 22
        x : 4
    }
    ListView {

        id : layerProperties
        height : parent.height - label.height
        width : parent.width

        highlight: Rectangle{ width : layerProperties.width; height : 20; color : Global.selectedColor}
        delegate  {
            Item {
                width : parent.width
                height : 20
                x : 4
                Text {
                    anchors.fill: parent
                    text : modelData.label
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            layerProperties.currentIndex = index
                        }
                    }
                }
            }
        }
        Component.onCompleted: {
            layerProperties.currentIndex = 0
        }
    }
}
