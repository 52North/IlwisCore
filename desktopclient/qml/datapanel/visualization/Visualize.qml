import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "./propertyeditors" as LayerManagement
import "../../controls" as Controls
import "../../Global.js" as Global
//import GeoDrawer 1.0


Item {
    id : layerview
    width: parent.width
    height : parent.height

    onWidthChanged: {
        console.debug(height, width)

    }

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
        if ( sourceUrl !== ""){
            layers.addDataSource(sourceUrl, sourceName, sourceType)
            viewmanager.addDataSource(sourceUrl, sourceName, sourceType)
         }
    }

    function transfer(datapanel){
        layers.transfer(datapanel)
    }

    Action {
        id : zoomClicked
        onTriggered : {
            if ( manager){
                manager.zoomInMode = !manager.zoomInMode

            }
        }
    }

    Action {
        id : entireClicked
        onTriggered : {
            layers.entireMap()
            viewmanager.entireMap()
        }
    }
    Action {
        id : refreshClicked
        onTriggered: {

        }
    }
    Action {
        id : zoomOutClicked
        onTriggered : {
            if ( manager){
                layer
                var envelope = layers.drawer().attributeOfDrawer("rootdrawer","zoomenvelope");
                if ( envelope !== ""){
                    var parts = envelope.split(" ")
                    console.debug(envelope)
                    var x1 = parseFloat(parts[0])
                    var y1 = parseFloat(parts[1])
                    var x2 = parseFloat(parts[3])
                    var y2 = parseFloat(parts[4])
                    var dx = 1.414 * Math.abs(x1 - x2)
                    var dy = 1.414 * Math.abs(y1 - y2)
                    var nx1 = (x2 + x1) / 2.0 - dx
                    var nx2 = (x2 + x1) / 2.0 + dx
                    var ny1 = (y2 + y1) / 2.0 - dy
                    var ny2 = (y2 + y1) / 2.0 + dy
                    var newenvelope = nx1 + " " + ny1 + " " + nx2 + " " + ny2
                    layers.newExtent(newenvelope)
                    viewmanager.newZoomExtent(newenvelope)
                }
            }
        }
    }

    LayerExtentsToolbar{
        id : maptools
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

            Component.onCompleted: {
                console.debug("layers", width, parent.height, maptools.height, height)
            }

        }
        ViewManager{
            id : viewmanager
            height : 150
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.right: parent.right
            renderer: layers.drawer()
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
