import QtQuick 2.1
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import WorkflowModel 1.0
import QtQuick.Dialogs 1.1
import ".." as Modeller
import "../../../Global.js" as Global

MouseArea {
    id: mousearea
    anchors.fill: parent
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    hoverEnabled: true
    property bool positionChanged: false
    property int lastX: width/2;
    property int lastY: height/2;
    property int lastOpX: width/2;
    property int lastOpY: height/2;

    onWheel: {
        var delta = wheel.angleDelta.y / 40
        zoom(delta, false, mouseX, mouseY)
    }
    onPressed: {
        selectThing()

    }
    onDoubleClicked: {
        openWorkflow()
    }
    onPositionChanged: {
        move(mouseX, mouseY)
    }
    onReleased: {
        wfCanvas.stopWorkingLine()
        oldx = -1
        oldy = -1
        cursorShape = Qt.ArrowCursor

        if ( currentItem && currentItem.type === "rangeitem"){
            currentItem.handleClick(mouseX, mouseY)
        }
        if ( currentItem && currentItem.type === "operationitem"){
            if ( currentItem.range)    {
                currentItem.range.resize()
            }
        }

        if ( currentItem && currentItem.type === "rangejunctionitem"){
            if ( currentItem.condition)    {
                currentItem.condition.resize()
            }
        }

        if( workarea.dropCondition){
            var component = Qt.createComponent("ConditionItem.qml");
            if (component.status === Component.Ready){
                var nodeid = workflow.addNode(0,{x : mouseX, y:mouseY, w:360, h:160,type:'conditionnode'})
                currentItem = component.createObject(wfCanvas, {"x": mouseX, "y": mouseY, "h" : 160, "w" : 360,"itemid" : nodeid, "scale": wfCanvas.scale});
                workarea.conditionsList.push(currentItem)
            }
            workarea.dropCondition = false
        }
        if( workarea.dropLoop){
            component = Qt.createComponent("RangeItem.qml");
            if (component.status === Component.Ready){
                nodeid = workflow.addNode(0,{x : mouseX, y:mouseY, w:360, h:160,type:'rangenode'})
                currentItem = component.createObject(wfCanvas, {"x": mouseX, "y": mouseY, "h" : 160, "w" : 360,"itemid" : nodeid, "scale": wfCanvas.scale});
                workarea.rangesList.push(currentItem)
            }
            workarea.dropLoop = false
        }
        wfCanvas.canvasValid = false
        workarea.selectState = false
    }

    function selectThing(){
        workarea.selectState = true
        if (canvasActive) {
            var operationSelected = -1, highestZ = -1, smallestDistance = 100000, selectedFlow=null
            var alllist = operationsList
            alllist  = alllist.concat(conditionsList)
            alllist = alllist.concat(rangesList)
            for(var k=0; k<conditionsList.length; ++k){
                alllist = alllist.concat( conditionsList[k].junctionsList)
            }
            for(var k=0; k<rangesList.length; ++k){
                alllist = alllist.concat( rangesList[k].junctionsList)
            }

            currentItem = itemAt(mouseX, mouseY)

            for(var i=0; i < alllist.length; ++i){

                var item = alllist[i]
                item.isSelected = false


                for(var j=0; j < item.flowConnections.length; j++)
                {
                    var flow = item.flowConnections[j];

                    // Retrieve basic X and Y positions of the line
                    if ( flow){
                        var startPoint = flow.attachsource.center();
                        var index = -1
                        if ( flow.target.type !== "conditionitem"){
                            index = flow.attachtarget
                        }
                        var endPoint = flow.target.attachementPoint(wfCanvas,index)
                        var x1 = startPoint.x;
                        var y1 = startPoint.y;
                        var x2 = endPoint.x;
                        var y2 = endPoint.y;
                        var mx = mouseX
                        var my = mouseY
                        var dx1 = Math.abs(x1 - mx)
                        var dx2 = Math.abs(x2 - mx)
                        var dy1 = Math.abs(y1 - my)
                        var dy2 = Math.abs(y2 - my)
                        var dy = y2 - y1
                        var dx = x2 - x1
                        if ( dx2 < Math.abs(dx) && dx1 < Math.abs(dx) && dy2 < Math.abs(dy) && dy1 < Math.abs(dy)){
                            var v1 = Math.abs((dy) * mx - (dx)*my + x2*y1 - y2*x1)
                            var v2 = Math.sqrt(Math.pow(dy,2) + Math.pow(dx,2))
                            var d = v1/v2
                            if(d < smallestDistance && d < 8)
                            {
                                smallestDistance = d;
                                selectedFlow = flow;
                            }
                        }
                        flow.isSelected = false;
                    }
                }
            }
           if ( selectedFlow){ // we might have selected a flow inside a range or condition. In that case we ignore the container
               currentItem = null
           }
        }

        oldx = mouseX
        oldy = mouseY

        if(!selectedFlow && !currentItem)
        {
            lastX = mouseX;
            lastY = mouseY;
            lastOpX = mouseX;
            lastOpY = mouseY;
        }

        if (selectedFlow && !currentItem) {
            selectedFlow.isSelected = true
            currentItem = selectedFlow
            workarea.updateMetadata()
        } else if (currentItem) {
            currentItem.isSelected = true
            workarea.showSelectedOperation(currentItem)

        }else {
            workarea.showSelectedOperation(null)
        }
        wfCanvas.canvasValid = false
    }

    function openWorkflow() {
        var operationSelected = -1, item = 0, highestZ = 0;
        for(var i=0; i < operationsList.length; ++i){

            item = operationsList[i]
            var isContained = mouseX >= item.x && mouseY >= item.y && mouseX <= (item.x + item.width) && mouseY <= (item.y + item.height)

            if ( isContained && item.z > highestZ ) {
                operationSelected = i
                highestZ = item.z
            }
        }
        if (operationSelected > -1) {
            var resource = mastercatalog.id2Resource(item.operation.id, area)
            var filter = "itemid=" + resource.id
            bigthing.newCatalog(filter, "workflow",resource.url,"other")
        }
    }

    function getCursorShape(xrelative, yrelative) {
        var cshape = Qt.SizeAllCursor

        if ( currentItem.type === "conditionitem" || currentItem.type === "rangeitem"){
            if ( xrelative < 10){
                if ( yrelative > currentItem.height * wfCanvas.zoomScale - 10)
                    cshape = Qt.SizeBDiagCursor
                else
                    cshape = Qt.SizeHorCursor
            }else if (xrelative > currentItem.width * wfCanvas.zoomScale - 10){
                if ( yrelative > currentItem.height * wfCanvas.zoomScale - 10)
                    cshape = Qt.SizeFDiagCursor;
                else
                    cshape = Qt.SizeHorCursor
            }else
                if ( yrelative  > currentItem.height * wfCanvas.zoomScale - 10)
                    cshape = Qt.SizeVerCursor
        }

        return cshape

    }

    function scaleMousePosition(num){
        if ( wfCanvas.zoomScale == 1)
            return num;
        var dzoom =  wfCanvas.zoomScale/wfCanvas.oldZoomScale
        var scaledNum = (num + width *(dzoom - 1))
        return scaledNum;
    }

    function move(xt, yt) {
        lastX = xt
        lastY = yt

        if (attachementForm.state === "invisible") {
            if (workingLineBegin.x !== -1) {
                workingLineEnd = Qt.point(mouseX, mouseY)
                canvasValid = false
            }
            else if (oldx >= 0 && oldy >= 0) {
                if ( currentItem ){
                    var xrelative = scaleMousePosition(mouseX) - currentItem.x
                    var yrelative = scaleMousePosition(mouseY) - currentItem.y
                    //scaleMousePosition(mouseX)
                    cursorShape = getCursorShape(xrelative, yrelative)
                    var prevXCorner = currentItem.x
                    var prevYCorner = currentItem.y
                    if ( cursorShape == Qt.SizeAllCursor){
                        currentItem.x += mouseX - oldx
                        currentItem.y += mouseY - oldy
                        if ( currentItem.type === "conditionitem"){
                            //currentItem.moveContent( mouseX - oldx, mouseY - oldy)
                            canvasValid = false
                        }
                        else if ( currentItem.type === "junctionitem"){
                            canvasValid = false
                        }
                    }else if (cursorShape == Qt.SizeHorCursor ){
                        if ( xrelative < 10){
                            currentItem.x = mouseX
                            currentItem.width = currentItem.width + prevXCorner - currentItem.x
                        }else
                            currentItem.width = Math.max(100,mouseX - currentItem.x)
                    } else if (cursorShape == Qt.SizeBDiagCursor ){
                        currentItem.x = mouseX
                        currentItem.width = currentItem.width + prevXCorner - currentItem.x
                        currentItem.height = Math.max(100,mouseY - currentItem.y)
                    }else if (cursorShape == Qt.SizeFDiagCursor ){
                        currentItem.width = Math.max(100,mouseX - currentItem.x)
                        currentItem.height = Math.max(100,mouseY - currentItem.y)
                    }else if (cursorShape == Qt.SizeVerCursor ){
                        if ( yrelative > currentItem.height - 10){
                            currentItem.height = Math.max(100,mouseY - currentItem.y)
                        }
                    }
                }else {
                    var deltax = oldx - mouseX
                    var deltay = oldy - mouseY
                    workarea.pan(deltax,deltay)
                }
                oldx = mouseX
                oldy = mouseY
            }
        }
    }
}

