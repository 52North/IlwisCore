import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../visualization" as MapTools
import LayersView 1.0

Rectangle {
    anchors.fill: parent

    Action {
        id : zoomClicked
        onTriggered : {
            if ( renderer.manager){
                renderer.manager.zoomInMode = !renderer.manager.zoomInMode
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

    MapTools.LayerExtentsToolbar{
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


        MapTools.LayerExtentMouseActions{
            id : mouseActions
            layerManager: renderer.manager
        }

        Component.onCompleted: {
            manager = uicontext.createLayerManager(objectName)
            renderer.setManager(manager)
            renderer.addCommand("adddrawer(" + renderer.viewerId + ",country_boundaries,ilwis://system/country_boundaries.ilwis,linecoverage)")
            renderer.addCommand("setlinecolor(" + renderer.viewerId + ", 0,darkblue)");
            renderer.associate(objectName,"drawEnded")
            renderer.associate(objectName,"drawEnded")
            renderer.update()

        }

        function finalizeDraw(){
            console.debug("dddd")
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
        property var items : []
        property var ctx

        function clear() {
            ctx.reset();
            ctx.clearRect(0, 0, width, height);
            ctx.stroke();
            mapItems.requestPaint();
        }

        onPaint: {
            if (!mapItems.ctx && mapItems.available){
                mapItems.ctx = mapItems.getContext('2d')
            }
            if (ctx  ) {
                clear(ctx);
                canvasDirty = false
                var l = items.length
                for (var i = 0; i < l; i++) {
                    ctx.save()
                    var envelope = items[i].drawEnvelope()
                    if ( envelope.width > 5 && envelope.height > 5){
                        ctx.beginPath();
                        ctx.lineWidth = 1;
                        ctx.strokeStyle = "red"
                        ctx.strokeRect(envelope.minx, envelope.miny, envelope.width, envelope.height)
                        ctx.text(items[i].name,envelope.minx + 5, envelope.miny + 10)
                        ctx.stroke()
                    }
                    ctx.restore()

                }
            }
        }
    }

}

