import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global

Rectangle {
    property var currentCoverage
    property var attributeListModel : currentCoverage ? currentCoverage.visualAttributes : null
    property alias currentIndex: attributesList.currentIndex

    function iconSource(name) {
        if ( name === "")
            return ""
         var iconP = "../../../images/" + name
         return iconP

     }
    color : Global.alternatecolor3

    Rectangle {
        id : attributesLabel
        width : parent.width + 10
        height : 18
        color : Global.alternatecolor3
        Text{
            text : qsTr("Data Attributes")
            font.weight: Font.DemiBold
            x : 5
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Rectangle {
        id : attributeListColumn
        anchors.top: attributesLabel.bottom
        anchors.topMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        width: parent.width - 8
        radius : 4
        color : "transparent"
        x : 8

        ListView {

            id : attributesList
            model : attributeListModel

            onCurrentIndexChanged: {
                currentCoverage.activeAttributeIndex = currentIndex;
                editorListColumn.displayEditorModel = attributesList.model[currentIndex].propertyEditors
                if ( editorListColumn.displayEditorModel.length > 0)
                    editorColumn.currentEditor = editorListColumn.displayEditorModel[0]
                else
                   editorColumn.currentEditor = null
            }

            Component {
                id: attributeHighlight

                Rectangle {
                    width: attributesList.width - 6; height: 14
                    x : 3
                    color: Global.selectedColor; radius: 2
                    y: (attributesList && attributesList.currentItem) ? attributesList.currentItem.y : 0
                }
            }
            anchors.fill: parent
            clip : true
            highlight: attributeHighlight
            delegate: Component {
                Loader {
                    sourceComponent: Component {
                        Item{
                            width : 100
                            height : 16
                            Image{
                                id : domicon
                                source : iconSource(icon)
                                width : 16
                                height : 16
                            }

                            Text {
                                x : 4
                                text: attributename
                                anchors.left : domicon.right
                                anchors.leftMargin: 2
                                width : 100
                                height : 14
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: {
                                        attributesList.currentIndex = index
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

