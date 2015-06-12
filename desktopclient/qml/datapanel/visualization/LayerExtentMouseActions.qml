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
    property bool showInfo : false
    signal zoomEnded()

    Controls.FloatingRectangle{
        id : floatrect
    }

    onPressed: {
        if ( layerManager.zoomInMode ){
            if ( !layerManager.hasSelectionDrawer){
                var position = {initialx: mouseX, initialy:mouseY}
                layerManager.hasSelectionDrawer = true
                drawer.addCommand("adddrawer(" + drawer.viewerId + ",selectiondrawer)")
                drawer.setAttribute("SelectionDrawer", position)
                drawer.update()
            }

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
        var mposition = mouseX + "|" + mouseY
        drawer.currentCoordinate = mposition
        if ( layerManager.hasSelectionDrawer){
            var position = {currentx: mouseX, currenty:mouseY}
            drawer.setAttribute("SelectionDrawer", position)
            drawer.copyAttribute("SelectionDrawer","envelope");
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
            drawer.addCommand("removedrawer(" + drawer.viewerId + ",selectiondrawer,post)");
            if ( envelope !== ""){
                linkedDrawer.addCommand("setviewextent("+ linkedDrawer.viewerId + "," + envelope + ")");

            }
            zoomEnded()
            layerManager.hasSelectionDrawer = false
            linkedDrawer.update()
        }
        floatrect.enabled = false
        floatrect.opacity = 0
        floatrect.x = 0
        floatrect.y = 0
    }
}


