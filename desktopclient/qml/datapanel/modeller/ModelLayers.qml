import QtQuick 2.0
import "../../Global.js" as Global

Item {
    property alias currentIndex : layerColumn.currentIndex
    Text {
        id : label
        text : qsTr("Model Layers")
        font.bold: true
        height : 22
        x : 4
    }

    ListView{
        id : layerColumn
        anchors.top : label.bottom
        anchors.bottom: parent.bottom
        width : parent.width
        model : layerprops.layernames
        highlight: Rectangle{ width : 200; height : 20; color : Global.selectedColor}
        delegate  {
            Item {
                width : parent.width
                height : 20
                x : 4
                Text {
                    anchors.fill: parent
                    text : modelData
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            layerColumn.currentIndex = index
                        }
                    }
                }
            }
        }

        Component.onCompleted: {
            currentIndex = -1
        }

    }
}
