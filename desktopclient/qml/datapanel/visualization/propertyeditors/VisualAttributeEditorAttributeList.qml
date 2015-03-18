import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global

Item {
    property var currentCoverage

    onCurrentCoverageChanged: {
        attributesList.model = currentCoverage.visualAttributes
        if ( attributesList.model)
            editorListColumn.currentVisualAttribute = attributesList.model[attributesList.currentIndex]
    }

    Rectangle {
        id : attributesLabel
        width : parent.width + 10
        height : 18
        color : Global.alternatecolor3
        Text{
            text : qsTr("Attributes")
            font.weight: Font.DemiBold
            x : 5
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Rectangle {
        id : attributeListColumn
        color : Global.alternatecolor2
        border.color: "lightgrey"
        border.width: 1
        anchors.top: attributesLabel.bottom
        anchors.topMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        width: parent.width

        ListView {

            id : attributesList
            Component {
                id: attributeHighlight

                Rectangle {
                    width: attributesList.width - 6; height: 14
                    x : 3
                    color: Global.selectedColor; radius: 2
                    y: (attributesList && attributesList.currentItem) ? attributesList.currentItem.y : 0
                    Behavior on y {
                        SpringAnimation {
                            spring: 3
                            damping: 0.2
                        }
                    }
                }
            }
            anchors.fill: parent
            clip : true
            highlight: attributeHighlight
            delegate: Component {
                Loader {
                    sourceComponent: Component {
                        Text {
                            x : 4
                            text: attributename
                            width : 100
                            height : 14
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    attributesList.currentIndex = index
                                    editorListColumn.currentVisualAttribute = attributesList.model[index]
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

