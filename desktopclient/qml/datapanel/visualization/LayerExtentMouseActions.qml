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
    property LayersView linkedDrawer
    property MapScrollers mapScrollers
    property bool showInfo : false
    property bool hasPermanence : false
    property bool zoomStarted : false
    property bool panningStarted : false
    property int panningPrevMouseX : -1
    property int panningPrevMouseY : -1
    property int panningDirection : Global.panningReverse
    signal zoomEnded(string envelope)

    Controls.FloatingRectangle{
        id : floatrect
    }

    onPressed:  {
        //console.log("onPressed...")
        if ( layerview.tabmodel){
            if (!layerview.tabmodel.selected)
                layerview.tabmodel.selectTab()
        }
        if ( layerManager.zoomInMode ){
            if ( !zoomStarted){
                drawer.addCommand("removedrawer(" + drawer.viewerId + ",selectiondrawer,post)");
                if (drawer.viewerId !== linkedDrawer.viewerId)
                    linkedDrawer.addCommand("removedrawer(" + linkedDrawer.viewerId + ",selectiondrawer,post)");
                layerManager.hasSelectionDrawer = false
            }

            if ( !layerManager.hasSelectionDrawer){
                var position = {initialx: mouseX, initialy:mouseY}
                layerManager.hasSelectionDrawer = true
                drawer.addCommand("adddrawer(" + drawer.viewerId + ",selectiondrawer)")
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
        if ( showInfo && drawer.showLayerInfo && !layerManager.hasSelectionDrawer){
          floatrect.enabled = true
          floatrect.opacity = 1
          floatrect.x = mouseX
          floatrect.y = mouseY
          var mposition = mouseX + "|" + mouseY
          floatrect.text = drawer.layerInfo(mposition)
        }

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
        if ( (zoomStarted || panningStarted) && layerManager.hasSelectionDrawer){
            var position;
            if (!panningStarted) {
                position = {currentx: mouseX, currenty:mouseY}
                drawer.setAttribute("selectiondrawer", position)
                drawer.copyAttribute("selectiondrawer","envelope");
            }
            else {
                position = {initialx: mouseX - 40, initialy: mouseY + 40}
                drawer.setAttribute("selectiondrawer", position)
                position = {currentx: mouseX + 40, currenty:mouseY - 40}
                drawer.setAttribute("selectiondrawer", position)
                //console.log("drag na overview")
                drawer.copyAttribute("selectiondrawer","envelope");
            }
            drawer.update()
        }
        if (panningStarted) {
            // todo: probably can be fine tuned, thus providing a smoother panning
            mapScrollers.vscroller.scroll(panningDirection * (mouseY - panningPrevMouseY))
            mapScrollers.hscroller.scroll(panningDirection * (mouseX - panningPrevMouseX))
            panningPrevMouseX = mouseX
            panningPrevMouseY = mouseY
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
            linkedDrawer.addCommand("setviewextent("+ linkedDrawer.viewerId + "," + envelope + ")");
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
            linkedDrawer.update()
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
            console.debug(wheel.angleDelta.y)
            var envelope = renderer.attributeOfDrawer("rootdrawer","zoomenvelope");
            Global.calcZoomOutEnvelope(envelope, layers, viewmanager, wheel.angleDelta.y < 0 ? 0.9 : -0.2 )
        }
    }
}


