import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

ToolBar{
    id : modellertools
    width : parent.width
    height : 55
    Column {
        anchors.fill: parent
        Row {
            width : parent.width
            height : 25
            spacing : 2
            id : editingtools

            Button {
                height : 25
                width : 25
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    source : iconsource("choice20.png")
                }
            }

            Button {
                height : 25
                width : 25
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    source : iconsource("deleteoperation20.png")
                }
                onClicked: {
                    modellerDataPane.deleteSelectedOperation()

                }
            }
            Button {
                id : conbut
                height : 25
                width : 25
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    source : iconsource("deleteconnection20.png")
                }
                onClicked: {
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

                    source : iconsource("zoomin20.png")
                }
                onClicked: {
                    modellerDataPane.canvasZoomIn()
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
                    modellerDataPane.canvasZoomOut()
                }
            }
        }
        Row {
            width : parent.width
            height : 25
            spacing : 2
            Button {
                height : 25
                width : 25
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    source : iconsource("run20.png")
                }
                onClicked : {
                    modellerDataPane.run()
                }

            }
            Button {
                height : 25
                width : 25
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    source : iconsource("step20.png")
                }
            }
            Button {
                height : 25
                width : 25
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    source : iconsource("pause20.png")
                }
            }
            Button {
                height : 25
                width : 25
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    source : iconsource("stop20.png")
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
}

