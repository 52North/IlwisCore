import QtQuick 2.2
import QtGraphicalEffects 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls

MouseArea {
    id : mapArea
    anchors.fill: parent
    hoverEnabled: true
    property LayerManager layerManager
    signal zoomEnded()

    Controls.FloatingRectangle{
        id : floatrect
    }

    onPressed: {
        if ( layerManager.zoomInMode ){
            if ( !layerManager.hasSelectionDrawer){
                var position = {initialx: mouseX, initialy:mouseY}
                layerManager.hasSelectionDrawer = true
                renderer.addCommand("adddrawer(" + renderer.viewerId + ",selectiondrawer)")
                renderer.setAttribute("SelectionDrawer", position)
                renderer.update()
            }

        }
        if ( renderer.showLayerInfo && !layerManager.hasSelectionDrawer){
          floatrect.enabled = true
          floatrect.opacity = 1
          floatrect.x = mouseX
          floatrect.y = mouseY
          var mposition = mouseX + "|" + mouseY
          floatrect.text = renderer.layerInfo(mposition)
        }

    }
    onPositionChanged: {
        var mposition = mouseX + "|" + mouseY
        renderer.currentCoordinate = mposition
        if ( layerManager.hasSelectionDrawer){
            var position = {currentx: mouseX, currenty:mouseY}
            renderer.setAttribute("SelectionDrawer", position)
            renderer.copyAttribute("SelectionDrawer","envelope");
            renderer.update()
        }
        if ( floatrect.opacity > 0){
            floatrect.x = mouseX
            floatrect.y = mouseY
            mposition = mouseX + "|" + mouseY
            floatrect.text = renderer.layerInfo(mposition)
        }
    }
    onReleased: {
        if ( layerManager.zoomInMode && layerManager.hasSelectionDrawer){
            var envelope = renderer.attributeOfDrawer("selectiondrawer","envelope");
            renderer.addCommand("removedrawer(" + renderer.viewerId + ",selectiondrawer,post)");
            if ( envelope !== ""){
                renderer.addCommand("setviewextent("+ renderer.viewerId + "," + envelope + ")");

            }
            zoomEnded()
            layerManager.hasSelectionDrawer = false
            renderer.update()
        }
        floatrect.enabled = false
        floatrect.opacity = 0
        floatrect.x = 0
        floatrect.y = 0
    }
}


