import QtQuick 2.2
import QtGraphicalEffects 1.0
import UIContextModel 1.0
import LayerManager 1.0
import LayersView 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

MouseArea {
    id : mapArea
    anchors.fill: parent
    hoverEnabled: true
    property LayerManager layerManager
    property LayersView drawer
    property bool zoomToExtents : true
    property MapScrollers mapScrollers
    property bool showInfo : false
    property bool hasPermanence : false
    property bool zoomStarted : false
    property bool panningStarted : false
    property int panningPrevMouseX : -1
    property int panningPrevMouseY : -1
    property int panningDirection : Global.panningReverse
    property string selectiondrawerColor : "basic"
    property string subscription
    signal zoomEnded(string envelope)

    Controls.FloatingRectangle{
        id : floatrect
    }

    onPressed:  {
        if ( layerview.tabmodel){
            if (!layerview.tabmodel.selected)
                layerview.tabmodel.selectTab()
        }
        if ( layerManager.zoomInMode ){
            if ( !zoomStarted){
                drawer.addCommand("removedrawer(" + drawer.viewerId + ",selectiondrawer,post)");
                layerManager.hasSelectionDrawer = false
            }

            if ( !layerManager.hasSelectionDrawer){ // overview
                var position = {initialx: mouseX, initialy:mouseY}
                layerManager.hasSelectionDrawer = true

                drawer.addCommand("adddrawer(" + drawer.viewerId + ",selectiondrawer, "+ selectiondrawerColor +")")
                drawer.setAttribute("selectiondrawer", position)
                drawer.update()
            }
            zoomStarted = true
        }

        if ( layerManager.panningMode ){
            panningStarted = true
            panningPrevMouseX = mouseX
            panningPrevMouseY = mouseY
            cursorShape = Qt.ClosedHandCursor

        }
        if ( showInfo && drawer.showLayerInfo && !zoomStarted){
          floatrect.enabled = true
          floatrect.opacity = 1
          floatrect.x = mouseX
          floatrect.y = mouseY
          var mposition = mouseX + "|" + mouseY
          floatrect.text = drawer.layerInfo(mposition)
        }
    }

    function panMapArea(dX, dY) {
        mapScrollers.vscroller.scroll(panningDirection * dY, true)
        mapScrollers.hscroller.scroll(panningDirection * dX, true)
    }

    onPositionChanged: {
        if (!layerManager.panningMode) {
            cursorShape = Qt.ArrowCursor
        } else {
            if (!panningStarted)
                cursorShape = Qt.OpenHandCursor
            else
                cursorShape = Qt.ClosedHandCursor
        }

        var mposition = mouseX + "|" + mouseY
        drawer.currentCoordinate = mposition

        var dX = mouseX - panningPrevMouseX
        var dY = mouseY - panningPrevMouseY

        if ( (zoomStarted || panningStarted) && layerManager.hasSelectionDrawer){
            var position;

            if (!panningStarted) { // zooming.....
                position = {currentx: mouseX, currenty:mouseY}
                drawer.setAttribute("selectiondrawer", position)
                drawer.copyAttribute("selectiondrawer","envelope");
            }
            else {
                var selDrawerEnvelope = drawer.attributeOfDrawer("selectiondrawer","envelope");

                var x1,x2,y1,y2
                var parts = selDrawerEnvelope.split(" ")
                x1 = parseFloat(parts[0])
                y1 = parseFloat(parts[1])
                x2 = parseFloat(parts[2])
                y2 = parseFloat(parts[3])

                var screenpos = {x: mouseX, y: mouseY, z: 0}
                var georefScreenPos = layerManager.screen2Coord(screenpos)

                if (georefScreenPos.x >= x1 && georefScreenPos.x <= x2 && georefScreenPos.y >= y1 && georefScreenPos.y <= y2) {
                    panMapArea(dX, dY)

                    var nx1 = x1 + dX
                    var nx2 = x2 + dX
                    var ny1 = y1 - dY
                    var ny2 = y2 - dY

                    var newenvelope = nx1 + " " + ny1 + " " + nx2 + " " + ny2
                    viewmanager.newZoomExtent(newenvelope)
                    layerview.publish( subscription, { envelope: newenvelope, options : false} )
                    panningPrevMouseX = mouseX
                    panningPrevMouseY = mouseY
                }
            }
            drawer.update()
        }
        if ( showInfo && floatrect.opacity > 0){
            floatrect.x = mouseX
            floatrect.y = mouseY
            mposition = mouseX + "|" + mouseY
            floatrect.text = drawer.layerInfo(mposition)
        }
    }
    onReleased: {
        if ( layerManager.zoomInMode && layerManager.hasSelectionDrawer){
            var envelope = drawer.attributeOfDrawer("selectiondrawer","envelope");
            //var pixelarea = drawer.attributeOfDrawer("rootdrawer","pixelarea");

            if (zoomToExtents)
                drawer.addCommand("setviewextent("+ drawer.viewerId + "," + envelope + ")");

            var options
            if (!zoomToExtents)
                options = true // TODO: create some sort of options array here, instead....
            else
                options = false

            layerview.publish(subscription, { envelope: envelope, options : options} )

            if ( envelope !== "" && !hasPermanence){
                layerManager.hasSelectionDrawer = false
                drawer.addCommand("removedrawer(" + drawer.viewerId + ",selectiondrawer,post)");
            }
            if ( hasPermanence){
                var parm = {envelope : envelope}
                drawer.setAttribute("selectiondrawer", parm)
                drawer.update()
            }
            zoomEnded(envelope)

            zoomStarted = false
        }
        if ( layerManager.panningMode ){
            panningStarted = false
            panningPrevMouseX = -1
            panningPrevMouseY = -1
            cursorShape = Qt.OpenHandCursor
        }
        floatrect.enabled = false
        floatrect.opacity = 0
        floatrect.x = 0
        floatrect.y = 0
    }
    onWheel: {
        if ( manager){
            var envelope = renderer.attributeOfDrawer("rootdrawer","zoomenvelope");
            Global.calcZoomOutEnvelope(envelope, layers, viewmanager, wheel.angleDelta.y < 0 ? 0.9 : -0.2 )
        }
    }
}


