import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../../Global.js" as Global

DropArea {
    property alias currentIndex : namesColumn.currentIndex
    property alias workflowNamesModel : namesColumn.model
    id : dropArea


    Rectangle {
        id : label
        width : parent.width - 2
        height : 22
        color : uicontext.paleColor
        Text{
            text : qsTr("Available workflows")
            font.bold: true
            x : 3
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    function addWorkflow(ilwisobjectid){
        modellerDataPane.model.addWorkflow("itemid=" + ilwisobjectid)
        namesColumn.model = modellerDataPane.model.workflowNames
    }

    onDropped: {
        addWorkflow(drag.source.ilwisobjectid)
    }

    Rectangle {
        anchors.top : label.bottom
        anchors.bottom : parent.bottom
        width : parent.width
        color : uicontext.lightestColor

        Rectangle {
            width : parent.width - 2
            height : parent.height - buttons.height - 5
            color : "white"
            ListView{
                id : namesColumn
                anchors.fill : parent
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
                                    namesColumn.currentIndex = index
                                }
                            }
                        }
                    }
                }

                Component.onCompleted: {
                    currentIndex = 0
                }

            }
        }
        Row {
            id : buttons
            width : childrenRect.width
            height: 22
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 2
            Button{
                id : deleteButton
                height : 18
                width: 35
                opacity : enabled ? 1 : 0.5
                enabled : true
                Image{
                    source : "../../../images/minus.png"
                    width : 14
                    height : 14
                    anchors.centerIn: parent
                }
                onClicked: {
                    modellerDataPane.model.removeWorkflow(namesColumn.currentIndex)
                    namesColumn.model = modellerDataPane.model.workflowNames
                }

            }
            Button{
                id : addButton
                height : 18
                width: 35
                Image{
                    source : "../../../images/plus.png"
                    width : 14
                    height : 14
                    anchors.centerIn: parent
                }
                onClicked: {
                    createForm.state = "visible"
                }

            }
        }
    }
}
