import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
//import GeoDrawer 1.0


Item {
    id : layerview
    width: parent.width
    height : parent.height
    objectName: uicontext.uniqueName()
    property LayerManager manager
    property bool canSeparate : true

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../../images/" + name
        return iconP
    }

    function addDataSource(sourceUrl, sourceName, sourceType){
        layers.addDataSource(sourceUrl, sourceName, sourceType)
        //layertools.model = manager.layers
    }

    function transferLayers(layermanager){
        layers.transferLayers(layermanager)
    }

    Action {
        id : zoomClicked
        onTriggered : {
            if ( manager){
                manager.zoomInMode = !manager.zoomInMode
                zoominButton.imageSource = iconsource(manager.zoomInMode ? "zoomin20A.png" : "zoomin20.png")
                zoominButton.checked = !zoominButton.checked
            }
        }
    }

    Action {
        id : entireClicked
        onTriggered : {
            layers.entireMap()
        }
    }

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
            onClicked: {
                layers.entireMap()
            }
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
            property string imageSource :  "zoomin20.png"
            Image {
                id : zoomimage
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                source : iconsource(zoominButton.imageSource)
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
    SplitView {
        anchors.top : maptools.bottom
        width : parent.width
        orientation: Qt.Vertical
        height : parent.height - maptools.height
        Layers{
            width : parent.width
            height : parent.height - maptools.height - 150
            id : layers
            Layout.fillWidth: true

        }
        LayerPropertyManagement{
            height : 150
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }

    Component.onCompleted: {
         manager = uicontext.createLayerManager(objectName)
        layers.setManager(manager)

    }
    Component.onDestruction: {
        // TODO : remove current VisualizationManager
    }

}
