import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Rectangle {
    id : modellayers
    width : parent.width
    height : parent.height

    SplitView {
        id : layerprops
        property var layernames : ["Definition view","Operational view","Template view", "Workflow view"]
        property var workflowitems : ["Workflow form", "Selected operation form", "Metadata selected form"]
        width : parent.width - 5
        height : parent.height
        y : 2

        function getItems(){
            if ( layerColumn.currentIndex == 3)
                return workflowitems
            return null
        }
        handleDelegate: Controls.SplitHandle{
            imageHeight: 15
        }

            Item {
                height : parent.height
                width : 140
                anchors.left: parent.left
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
                highlight: Rectangle{ width : parent.width; height : 20; color : Global.selectedColor}
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
                onCurrentIndexChanged: {
                    datapaneChanged(currentIndex)
                }

                Component.onCompleted: {
                    currentIndex = 3
                }

            }
        }
        ListView {
            id : layerProperties
            width : 230
            height : parent.height
            model : layerprops.getItems()
            highlight: Rectangle{ width : parent.width; height : 20; color : Global.selectedColor}
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
                                layerProperties.currentIndex = index
                            }
                        }
                    }
                }
            }
        }
    }
}

