import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

ToolBar{
    id : modellertools
    width : parent.width
    height : 35
    Row {
        anchors.fill: parent
        anchors.verticalCenter: parent.verticalCenter
        spacing : 2
        Button {
            id : conbut
            height : 25
            width : 25
            checkable: true
            checked: false
            Image {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                source : iconsource(conbut.checked ? "connectionselect20.png" : "connection20.png")
            }
        }

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
        }
        Button {
            height : 25
            width : 25
            Image {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                source : iconsource("zoomin20.png")
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
        }
    }
}

