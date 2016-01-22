import QtQuick 2.2
import QtGraphicalEffects 1.0
import UIContextModel 1.0
import LayerManager 1.0
import LayersView 1.0

MouseArea {
    id : mapArea
    anchors.fill: parent
    hoverEnabled: true
    property LayerManager layerManager
    property LayersView drawer
    property bool zoomStarted : false
    signal zoomEnded(string envelope)

    FloatingRectangle{
        id : floatrect
    }

    onPressed:  {
        if ( layerManager.zoomInMode ){
            if ( !zoomStarted){
                drawer.addCommand("removedrawer(" + drawer.viewerId + ",selectiondrawer,post)");
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

    }
    onPositionChanged: {
        var mposition = mouseX + "|" + mouseY
        drawer.currentCoordinate = mposition
        if ( zoomStarted && layerManager.hasSelectionDrawer){
            var position = {currentx: mouseX, currenty:mouseY}
            drawer.setAttribute("selectiondrawer", position)
            drawer.copyAttribute("selectiondrawer","envelope");
            drawer.update()
        }
    }
    onReleased: {
        if ( layerManager.zoomInMode && layerManager.hasSelectionDrawer){
            var envelope = drawer.attributeOfDrawer("selectiondrawer","envelope");
            drawer.addCommand("setviewextent("+ drawer.viewerId + "," + envelope + ")");
            if ( envelope !== "" ){
                layerManager.hasSelectionDrawer = false
                drawer.addCommand("removedrawer(" + drawer.viewerId + ",selectiondrawer,post)");

            }
            zoomEnded(envelope)
            zoomStarted = false
            drawer.update()
        }
        floatrect.enabled = false
        floatrect.opacity = 0
        floatrect.x = 0
        floatrect.y = 0
    }
}




