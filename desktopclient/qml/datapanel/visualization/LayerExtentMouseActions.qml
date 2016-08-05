import QtQuick 2.2
import QtGraphicalEffects 1.0
import UIContextModel 1.0
import LayerManager 1.0
import LayersView 1.0
import "../../controls" as Controls

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
    signal zoomEnded(string envelope)

    Controls.FloatingRectangle{
        id : floatrect
    }

    onPressed:  {
        console.log("onPressed 0")
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
            //console.log("onPressed event: panningMode")
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
            var position = {currentx: mouseX, currenty:mouseY}
            drawer.setAttribute("selectiondrawer", position)
            drawer.copyAttribute("selectiondrawer","envelope");
            drawer.update()
        }
        if (panningStarted) {
            // todo: probably can be fine tuned, thus providing a smoother panning
            mapScrollers.vscroller.scroll(-(mouseY - panningPrevMouseY))
            mapScrollers.hscroller.scroll(-(mouseX - panningPrevMouseX))
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
        console.log("0");
        if ( layerManager.zoomInMode && layerManager.hasSelectionDrawer){
            console.log("1");
            var envelope = drawer.attributeOfDrawer("selectiondrawer","envelope");
            console.log("2");
            linkedDrawer.addCommand("setviewextent("+ linkedDrawer.viewerId + "," + envelope + ")");
            console.log("3");
            if ( envelope !== "" && !hasPermanence){
                console.log("4");
                layerManager.hasSelectionDrawer = false
                console.log("5");
                drawer.addCommand("removedrawer(" + drawer.viewerId + ",selectiondrawer,post)");
                console.log("6");

            }
            console.log("7");
            if ( hasPermanence){
                console.log("8");
                var parm = {envelope : envelope}
                console.log("9");
                drawer.setAttribute("selectiondrawer", parm)
                console.log("10");
                drawer.update()
                console.log("11")
            }
            console.log("12");
            zoomEnded(envelope)
            console.log("13");
            zoomStarted = false
            console.log("14");
            linkedDrawer.update()
            console.log("15");
        }
        if ( layerManager.panningMode ){
            //console.log("onReleased event: panningMode")
            panningStarted = false
            panningPrevMouseX = -1
            panningPrevMouseY = -1
            cursorShape = Qt.OpenHandCursor
        }
        console.log("16");
        floatrect.enabled = false
        floatrect.opacity = 0
        floatrect.x = 0
        floatrect.y = 0
        console.log("17");
    }
}


