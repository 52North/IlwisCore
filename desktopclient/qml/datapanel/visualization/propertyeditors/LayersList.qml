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
        height : parent.height - buttonbar.height - 14
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
                objectName: uicontext.uniqueName()
                model : manager.layers
                anchors.fill: parent
                anchors.margins: 4
                delegate: LayerDelegate{}
                highlight: highlight
                focus: true
                clip : true

                Component.onCompleted: {
                    manager.setLayerListName(layersList.objectName)
                }

            }
        }

    }
    Row {
        id : buttonbar
        height : 18
        width : firstColumn.width
        anchors.top : layersContainer.bottom
        anchors.topMargin: 4
        opacity : layersList.model.length > 2 ? 1 : 0
        enabled : layersList.model.length > 2 ? 1 : 0
        Text {
            text : qsTr("Layer")
            width : 31
            anchors.verticalCenter: parent.verticalCenter
        }

        Button{
            //text :qsTr("up")
            width : 35
            height : 18
            Image{
                source : "../../../images/arrowdown.png"
                rotation: 180
                width : 12
                height : 12
                anchors.centerIn: parent

            }
            onClicked: {
                var index = layersList.currentIndex
                if ( index >= 1){
                    index = index - 1
                    renderer.addCommand("layermanagement("+ renderer.viewerId + "," + index + ",layerup)")
                    renderer.update()
                }

            }
        }


        Button{
            height : 18
            width : 35
            Image{
                source : "../../../images/arrowdown.png"
                width : 12
                height : 12
                anchors.centerIn: parent
            }
            onClicked: {
                var index = layersList.currentIndex
                if ( index >= 1){
                    index = index - 1
                    var command = "layermanagement("+ renderer.viewerId + "," + index + ",layerdown)"
                    renderer.addCommand(command)
                    renderer.update()
                }
            }
        }
        Button{
            id : deleteButton
            height : 18
            width: 35
            opacity : layersList.height > 5 ? 14 : 0
            enabled : layersList.height > 5
            Image{
                source : "../../../images/minus.png"
                width : 14
                height : 14
                anchors.centerIn: parent
            }
            onClicked: {
                var index = layersList.currentIndex
                if ( index >= 1){
                    index = index - 1
                    renderer.addCommand("layermanagement("+ renderer.viewerId + "," + index + ",layerremove)")
                    renderer.update()
                }
            }

        }
    }

}
