import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../visualization" as MapTools
import "../../Global.js" as Global
import LayersView 1.0

Rectangle {
    width : parent.width
    id : catalogMapView

    Action {
        id : zoomClicked
        onTriggered : {
            if ( renderer.manager){
                renderer.manager.zoomInMode = !renderer.manager.zoomInMode
                grid.setSource("")
            }
        }
    }

    Action {
        id : zoomOutClicked
        onTriggered : {
            if ( renderer.manager){
                var envelope = renderer.attributeOfDrawer("rootdrawer","zoomenvelope");
                Global.calcZoomOutEnvelope(envelope, renderer, renderer.manager)
            }
        }
    }

    Action {
        id : entireClicked
        onTriggered : {
            var envelope = renderer.attributeOfDrawer("rootdrawer","coverageenvelope");
            if ( envelope !== ""){
                renderer.addCommand("setviewextent("+ renderer.viewerId + "," + envelope + ")");
            }
            renderer.update()
        }
    }

    Action {
        id : refreshClicked
        onTriggered: {
            renderer.viewEnvelope
            currentCatalog.prepareMapItems(renderer.manager, true)
            mapItems.items = currentCatalog.mapItems
            mapItems.canvasDirty = true
            mapItems.requestPaint()
        }
    }

    Controls.LayerExtentsToolbar{
        id : maptools
    }
    LayersView {
        id: renderer
        objectName : "mapcatalog_mainui"
        property LayerManager manager
        width : parent.width
        anchors.top : maptools.bottom
        anchors.bottom: parent.bottom
        anchors.margins: 5

        function newExtent(ext){
            addCommand("setviewextent("+ viewerId + "," + ext + ")");
            update()
        }
        Connections {
            target : mouseActions
            onClick :{
                var maps = []
                for (var i = 0; i < mapItems.items.length; i++) {
                    var env = mapItems.items[i].getProperty("latlonenvelope")
                    if ( env === "?")
                        continue;

                    var envelope = renderer.drawEnvelope(env)
                    if ( envelope.minx <=x && (envelope.minx + envelope.width) > x &&
                            envelope.miny <= y && (envelope.miny + envelope.height) > y ){
                        maps.push({"name" : mapItems.items[i].name,
                                      "imagePath" : mapItems.items[i].imagePath,
                                      "id" : mapItems.items[i].id,
                                      "iconPath" : mapItems.items[i].iconPath,
                                      "url" : mapItems.items[i].url,
                                      "typeName" : mapItems.items[i].typeName })
                    }
                }
                if ( maps.length > 0 && !renderer.manager.zoomInMode){
                    grid.setSource("") // remove old grid
                    grid.setSource("SelectedSpatialItems.qml",{"x" : x + 20, "y" : y -20, "model" : maps})
                    grid.active  = true
                }
                if (catalogViews && catalogViews.tabmodel && !catalogViews.tabmodel.selected)
                    catalogViews.tabmodel.selectTab()
             }
        }

        Controls.LayerExtentMouseActions{
            id : mouseActions
            layerManager: renderer.manager
            drawer : renderer
        }

        Component.onCompleted: {
            manager = uicontext.createLayerManager(objectName)
            renderer.setManager(manager)
            var cmd = uicontext.worldmapCommand(renderer.viewerId)
            renderer.addCommand(cmd)
            renderer.addCommand("setlinecolor(" + renderer.viewerId + ", 0,darkblue)");
            renderer.associate(objectName,"drawEnded")
            renderer.update()

        }

        function finalizeDraw(){
            mapItems.canvasDirty = true
            mapItems.requestPaint()
        }

    }
    Canvas{
        id : mapItems
        width : parent.width
        anchors.top : maptools.bottom
        anchors.bottom: parent.bottom
        anchors.margins: 5
        property bool canvasDirty: false
        property var items : currentCatalog ? currentCatalog.mapItems : []
        property var ctx

        function clear() {
            ctx.reset();
            ctx.clearRect(0, 0, width, height);
            ctx.stroke();
            mapItems.requestPaint();
        }
        Loader {
            id : grid
        }

        onPaint: {
            if ( catalogMapView.height != 0){
                if (!mapItems.ctx && mapItems.available){
                    mapItems.ctx = mapItems.getContext('2d')
                }
                if (ctx && renderer.manager ) {
                    clear(ctx);
                    canvasDirty = false
                    var l = items.length



                    for (var i = 0; i < l; i++) {
                        ctx.save()
                        var env = items[i].getProperty("latlonenvelope")
                        if ( env === "?")
                            continue;

                        var envelope = renderer.drawEnvelope(env)
                        if ( envelope.width > 5 && envelope.height > 5){
                            ctx.beginPath();
                            ctx.lineWidth = 1;
                            ctx.strokeStyle = "red"
                            ctx.strokeRect(envelope.minx, envelope.miny, envelope.width, envelope.height)
                            ctx.text(items[i].name,envelope.minx + 5, envelope.miny + 10)
                            ctx.stroke()
                        }else {
                            ctx.beginPath();
                            ctx.lineWidth = 1;
                            ctx.strokeStyle = "red"
                            var xc = envelope.minx + envelope.width / 2
                            var yc = envelope.miny + envelope.height / 2
                            ctx.moveTo(xc - 5, yc)
                            ctx.lineTo(xc + 5, yc)
                            ctx.moveTo(xc, yc - 5)
                            ctx.lineTo(xc, yc + 5)
                            ctx.moveTo(xc,yc)
                            ctx.arc(xc,yc,5,0, Math.PI * 2,true)
                            ctx.stroke()

                        }

                        ctx.restore()

                    }

                }
            }
        }
    }

}

