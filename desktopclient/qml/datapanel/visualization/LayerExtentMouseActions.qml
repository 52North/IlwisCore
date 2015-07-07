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
    property bool hasPermanence : false
    property bool zoomStarted : false
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
                if (drawer.viewerId !== linkedDrawer.viewerId)
                    linkedDrawer.addCommand("removedrawer(" + linkedDrawer.viewerId + ",selectiondrawer,post)");
                layerManager.hasSelectionDrawer = false
            }

            if ( !layerManager.hasSelectionDrawer){
                var position = {initialx: mouseX, initialy:mouseY}
                layerManager.hasSelectionDrawer = true
                drawer.addCommand("adddrawer(" + drawer.viewerId + ",selectiondrawer)")
                drawer.setAttribute("SelectionDrawer", position)
                drawer.update()
            }
            zoomStarted = true

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
        if ( zoomStarted && layerManager.hasSelectionDrawer){
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
            linkedDrawer.addCommand("setviewextent("+ linkedDrawer.viewerId + "," + envelope + ")");
            if ( envelope !== "" && !hasPermanence){
                layerManager.hasSelectionDrawer = false
                drawer.addCommand("removedrawer(" + drawer.viewerId + ",selectiondrawer,post)");

            }
            if ( hasPermanence){
                var parm = {envelope : envelope}
                drawer.setAttribute("SelectionDrawer", parm)
                drawer.update()
            }
            zoomEnded(envelope)
            zoomStarted = false
            linkedDrawer.update()
        }
        floatrect.enabled = false
        floatrect.opacity = 0
        floatrect.x = 0
        floatrect.y = 0
    }
}


