import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

ToolBar{
    id : modellertools
    width : parent.width
    height : 31

    property alias zoomLevel: zoomLabel

    Column {
        height:parent.height
        width: parent.width/3
        id : editingColumn
        Row {
            width : parent.width
            height : 25
            spacing : 2
            id : editingtools

            Button {
                id : newcondition
                height : 25
                width : 25
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    source : iconsource("choice20.png")
                }
                onClicked: {
                    modellerDataPane.newCondition()

                }
            }

            Button {
                height : 25
                width : 25
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    source : iconsource("trash20.png")
                }
                onClicked: {
                    modellerDataPane.deleteSelectedOperation()
                    modellerDataPane.deleteSelectedEdge()

                }
            }
            Button {
                id : chanconbut
                height : 25
                width : 25
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    source : iconsource("refresh20.png")
                }
                onClicked: {
                    modellerDataPane.alterSelectedEdge()
                }
            }

            Button {
                height : 25
                width : 25
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    source : iconsource("error_sign.png")
                }
                onClicked: {

                    if(errorview.state == "smaller")
                    {
                        errorview.state = "bigger"
                    } else {
                        errorview.state = "smaller"
                    }

                }
            }
        }

    }


    Column{
        id:zoomTools
        anchors{
            right: parent.right
        }

        height: parent.height
        width: parent.width/3
        Row{
            height : 25
            spacing : 2
            anchors{
                right: parent.right
            }

            Button {
                height : 25
                width : 25
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    source : iconsource("zoomin20.png")
                }
                onClicked: {
                    modellerDataPane.canvasZoom(3)
                }
            }

            Button {
                height : 25
                width : 75
                text: "Default zoom"

                onClicked: {
                    modellerDataPane.defaultZoom()
                }
            }

            Button {
                height : 25
                width : 25
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    source : iconsource("zoomout20.png")
                }
                onClicked: {
                    modellerDataPane.canvasZoom(-3)
                }
            }

            Text{
                id:zoomLabel
                text:"100%"
                font.pixelSize: 18
            }
        }

    }
}

