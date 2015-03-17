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


Item {
    id : viewmanager

    property var renderer

    Layout.minimumHeight: 22

    Component {
        id : displayOptions
        DisplayOptions.LayerPropertyManagement{}
    }

    Component {
        id : layersinfo

        LayersInfo{}
    }

    Component{
        id : metadata
        MetaData{}
    }

    Rectangle {
        id : header
        width : parent.width
        height : 24
        anchors.top : parent.top
        color : Global.alternatecolor3
        TextField{
            id : nativeCoords
            width : 145;
            height : 18
            anchors.right: parent.right
            anchors.rightMargin: 4
            readOnly: true
            anchors.verticalCenter: parent.verticalCenter
            text : manager.currentLatLon
        }

        TextField{
            id : otherCoords
            width : nativeCoords.width;
            height : 18
            anchors.right: nativeCoords.left
            anchors.rightMargin: 4
            readOnly: true
            anchors.verticalCenter: parent.verticalCenter
            text : manager.currentCoordinate
        }
    }

    TabView{
        anchors.fill: parent

        Component.onCompleted: {
            var tab =addTab(qsTr("Display Options"), displayOptions)
            tab.active = true
            tab.item.renderer = renderer

            addTab(qsTr("Layers Info"), layersinfo)
            addTab(qsTr("Metadata"), metadata)
        }

        style: Base.TabStyle1{
        }
    }



}

