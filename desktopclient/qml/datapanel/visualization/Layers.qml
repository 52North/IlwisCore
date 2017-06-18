import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls
import LayersView 1.0

Item {
    id : layerContainer
    objectName: "layerContainer_ui"  + uicontext.uniqueName()
    property alias layerContainerId : layerContainer
    property alias mapScrollers : mapScrollers    
    property variant subscription
    signal zoomEnded(string envelope)

    property bool reportExtentsBack : false

    function entireMap() {
        maparea.entireMap()
    }

    function setManager(layermanager){
        maparea.drawer.setManager(layermanager)
    }

    function drawer(){
        return maparea.drawer
    }

    function maparea(){
        return maparea
    }

    function transfer(mappane){
        maparea.transfer(mappane)
    }

    function addDataSource(filter, sourceName, sourceType){
        var filter2 = filter
        if ( filter.indexOf("=") !== -1){
            filter = "\"" + filter + "\""
        }
        console.debug("addrawer 1")
        maparea.drawer.addCommand("adddrawer(" + maparea.drawer.viewerId + ","+ sourceName + "," + filter + "," + sourceType + ")")
        maparea.drawer.update()
        viewmanager.addDataSource(filter2, sourceName, sourceType)
        return maparea.drawer.viewerId
    }

    function newExtent(ext){
        maparea.drawer.addCommand("setviewextent("+ maparea.drawer.viewerId + "," + ext + ")");
        maparea.drawer.update()
    }

    function publishDrawerPixelarea() {
        var pixelarea = maparea.drawer.attributeOfDrawer("rootdrawer","pixelarea");
        if (pixelarea !== "") {
            var pixelareaparts = pixelarea.split(" ");

            var w = pixelareaparts[0];
            var h = pixelareaparts[1];

            var minPos = {x: 0, y: h, z: 0}
            var maxPos = {x: w, y: 0, z: 0}

            var minCoord = manager.screen2Coord(minPos)
            var maxCoord = manager.screen2Coord(maxPos)
            var shownEnvelope = minCoord.x + " " + minCoord.y + " " + maxCoord.x + " " + maxCoord.y

            // when multiple layers are present, this is needed while resizing the maparea
            newExtent(shownEnvelope);

            layerview.publish(layerview.selectiondrawertopicoverview, { envelope : shownEnvelope, options : false })
        }
    }

    function clbkSynchronizeEnded(){
        if (reportExtentsBack) {
            publishDrawerPixelarea()
            reportExtentsBack = false
        }

        // todo: fix this
        /*var coverageenvelope = maparea.drawer.attributeOfDrawer("rootdrawer","coverageenvelope");

        var parts = coverageenvelope.split(" ")
        var x1 = parseFloat(parts[0])
        var y1 = parseFloat(parts[1])
        var x2 = parseFloat(parts[2])
        var y2 = parseFloat(parts[3])
        // need to do this, so that the overview map is happy when layers are dropped on the main map
        maparea.drawer.viewEnvelope = {minx: x1, miny: y1, maxx: x2, maxy: y2}
        */
    }

    MapArea{
        anchors.fill : parent
        id : maparea

        onWidthChanged: {
            publishDrawerPixelarea()
        }

        onHeightChanged: {
            publishDrawerPixelarea()
        }
    }

    MapScrollers {
        anchors.fill: parent
        id : mapScrollers

        function selectionDrawerUpdate (topic, data) {
            newExtent(data.envelope)

            var env = {envelope : data.envelope, preserveaspectration : false}
            maparea.drawer.setAttribute("selectiondrawer", env )
            maparea.drawer.update()

            reportExtentsBack = (data.options === true)
        }

        Component.onCompleted: {
            maparea.mouseActions.mapScrollers = mapScrollers
            subscription = layerview.subscribe(layerview.selectiondrawertopic, selectionDrawerUpdate)
            maparea.mouseActions.subscription = layerview.selectiondrawertopicoverview //subscription
            mapScrollers.subscription = layerview.selectiondrawertopicoverview // subscription
            drawer().associate(layerContainer.objectName,"synchronizeEnded")
        }

        onScrolled: {
            maparea.drawer.setAttribute("selectiondrawer", {envelope : envelope} )
            maparea.drawer.update()
            layerview.publish(layerview.selectiondrawertopicoverview, { envelope : envelope, options : false })
        }
    }




}

