import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global


Item {
    id : firstColumn
    width : 180
    Item {
        id : layersContainer
        width : parent.width
        height : parent.height - deleteButton.height - 14
        Rectangle {
            id : layersLabel
            width : parent.width + 10
            height : 18
            color : Global.alternatecolor3
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
            height : parent.height - layersLabel.height - 3
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
                onCountChanged: {
                    currentIndex = layerIndex + 1 ; // the new layer will be on top, so the last index has shifted one up
                    var coverage = manager.layer(currentIndex)
                    if ( coverage){
                        attributeListColumn.currentCoverage = coverage
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
        text: qsTr("remove layer")
        anchors.top : layersContainer.bottom
        anchors.topMargin: 4
    }

}
