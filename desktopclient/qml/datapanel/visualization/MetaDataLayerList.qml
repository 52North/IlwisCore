import QtQuick 2.2
import "../../Global.js" as Global


ListView {

    Component {
        id: highlight

        Rectangle {
            width: layersList.width; height: 18
            color: Global.selectedColor; radius: 2
            y: (layersList && layersList.currentItem) ? layersList.currentItem.y : 0
        }
    }
    id : layersList
    model : manager.layers
    anchors.fill: parent
    anchors.margins: 4
    delegate: Component{
        Item {
            id : layerRow
            width: parent.width
            height: 18

            Row {
                spacing: 2
                width : parent.width
                Image {
                    id : image
                    width : 16; height :16
                    source : iconSource(iconPath)
                    fillMode: Image.PreserveAspectFit
                }
                Text{
                    text : name
                    width : parent.width - image.width
                    font.pointSize: 8
                    elide: Text.ElideMiddle
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            layersList.currentIndex = index
                            mastercatalog.setSelectedObjects(id)
                            bigthing.getWorkbenchPane("ObjectProperties.qml");
                        }
                    }
                }

            }
        }
    }

    highlight: highlight
    focus: true
    clip : true

}

