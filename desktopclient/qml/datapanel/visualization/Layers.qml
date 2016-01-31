import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls
import LayersView 1.0

Item {
    id : layerContainer
    signal zoomEnded(string envelope)



    function setManager(layermanager){
        maparea.drawer.setManager(layermanager)
    }

    function drawer(){
        return maparea.drawer
    }

    function addDataSource(filter, sourceName, sourceType){
        if ( filter.indexOf("=") !== -1){
            filter = "\"" + filter + "\""
        }
        maparea.drawer.addCommand("adddrawer(" + maparea.drawer.viewerId + ","+ sourceName + "," + filter + "," + sourceType + ")")
        maparea.drawer.update()
    }

    function newExtent(ext){
        maparea.drawer.addCommand("setviewextent("+ maparea.drawer.viewerId + "," + ext + ")");
        maparea.drawer.update()
    }

    MapArea{
        anchors.fill : parent
        id : maparea
    }

    MapScrollers {
        anchors.fill: parent
    }

//    PostMapPanelCanvas{
//        anchors.fill: parent

//        Component.onCompleted: {
//            addDrawer("drawers/GridDrawer.qml")
//        }
//    }
}

