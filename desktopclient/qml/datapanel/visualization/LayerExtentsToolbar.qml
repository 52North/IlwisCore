import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "./propertyeditors" as LayerManagement
import "../../controls" as Controls
import "../../Global.js" as Global

ToolBar{
    id : maptools
    width : parent.width
    height : 35
    Button {
        height : 25
        width : 25
        id : entireMap
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 2
        tooltip: "EntireMap"
        Image {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            source : iconsource("entiremap20.png")
        }
        action : entireClicked
    }

    Button {
        height : 25
        width : 25
        id : refreshButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.left :entireMap.right
        action : entireClicked
        anchors.rightMargin: 2
        Image {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            source : iconsource("refresh20.png")
        }
    }


    Button {
        height : 25
        width : 25
        id : panButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.left :refreshButton.right
        anchors.rightMargin: 2
        Image {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            source : iconsource("pan20.png")
        }
    }


    Button {
        height : 25
        width : 25
        id : zoominButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.left :panButton.right
        anchors.rightMargin: 2
        action : zoomClicked
        checkable: true
        checked: false
        Image {
            id : zoomimage
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            source : iconsource(zoominButton.checked ? "zoomin20A.png" : "zoomin20.png")
        }
        onClicked: {
            checked = !checked
        }
    }
    Button {
        height : 25
        width : 25
        id : zoomoutButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.left :zoominButton.right
        anchors.leftMargin: 2
        checkable: true
        checked: false
        Image {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            source : iconsource("zoomout20.png")
        }
    }
}


