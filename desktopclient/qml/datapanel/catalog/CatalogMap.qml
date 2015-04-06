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

    MapTools.LayerExtentsToolbar{
        id : maptools
    }
    LayersView {
        id: renderer
        property LayerManager manager
        width : parent.width
        anchors.top : maptools.bottom
        anchors.bottom: parent.bottom
        anchors.margins: 5

        Connections {
            target: mouseActions
            onZoomEnded :{
                mapItems.markDirty(Qt.rect(0, 0, mapItems.width, mapItems.height))
            }
        }

        MapTools.LayerExtentMouseActions{
            id : mouseActions
            layerManager: renderer.manager
        }

        Component.onCompleted: {
            manager = uicontext.createLayerManager(objectName)
            renderer.setManager(manager)
            renderer.addCommand("adddrawer(" + renderer.viewerId + ",country_boundaries,ilwis://system/country_boundaries.ilwis,linecoverage)")
            renderer.addCommand("setlinecolor(" + renderer.viewerId + ", 0,darkblue)");
            renderer.update()
        }
    }
    Canvas{
        id : mapItems
        width : parent.width
        anchors.top : maptools.bottom
        anchors.bottom: parent.bottom
        anchors.margins: 5

        onPaint: {
//            var llenv = renderer.envelope();
//            if ( typeof llenv.minx !== 'undefined' ){
//                var context = getContext("2d");
//                context.clearRect(0,0,width, height)
//                currentCatalog.prepareMapItems(renderer.manager)
//                var items = currentCatalog.mapItems
//                for(var i =0; i < items.length; ++i){
//                    var envelope = items[i].drawEnvelope()
//                    if ( typeof llenv.minx !== 'undefined'){
//                        if ( envelope.width > 5 && envelope.height > 5){
//                            context.beginPath();
//                            context.lineWidth = 2;
//                            context.strokeStyle = "red"
//                            context.strokeRect(envelope.minx, envelope.miny, envelope.width, envelope.height)
//                        }
//                    }
//                }
//            }
        }

    }
}

