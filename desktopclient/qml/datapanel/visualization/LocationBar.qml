import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "./propertyeditors" as DisplayOptions
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base

Rectangle {
    id : header
    width : 300
    anchors.right: parent.right
    height : 24
    anchors.verticalCenter: parent.verticalCenter

    color : "transparent"
    property var renderer

    TextField{
        id : nativeCoords
        width : 155;
        height : 18
        anchors.right: parent.right
        anchors.rightMargin: 4
        readOnly: true
        anchors.verticalCenter: parent.verticalCenter
        text : renderer ? renderer.currentLatLon : ""
    }

    TextField{
        id : otherCoords
        width : nativeCoords.width - 14;
        height : 18
        anchors.right: nativeCoords.left
        anchors.rightMargin: 4
        readOnly: true
        anchors.verticalCenter: parent.verticalCenter
        text : renderer ? renderer.currentCoordinate : ""
    }
}

