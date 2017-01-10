import QtQuick 2.1
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import WorkflowModel 1.0
import QtQuick.Dialogs 1.1
import ".." as Modeller
import "../../../matrix.js" as Matrix
import "../../../Global.js" as Global

Modeller.ModellerWorkArea {
    id : workarea
    property WorkflowModel workflow;
    property var workflowManager
    property OperationCatalogModel operationCatalog;
    property var operationsList: []
    property var conditionsList: []

    property var currentItem
    property bool canvasActive : true;
    property bool dropCondition : false

    onDropConditionChanged: {
        tools.dropCondition(dropCondition)
    }

    state : "visible"

    WorkflowTools{
        id : tools
        z : 10
    }

    DropArea {
        id: canvasDropArea
        anchors.top: tools.bottom
        width : parent.width
        height : parent.height - tools.height
        enabled: true

        function addOperation(objectid,dx,dy){
            var operation = operations.operation(objectid)
            if ( operation){
                var nodeId = workflow.addNode(objectid,{x : dx, y:dy, w:240, h:130,type:'operationnode'})
                wfCanvas.createItem(dx, dy,operation, nodeId)
            }



            if ( workflowManager){
                workflowManager.updateRunForm()
            }
            return operation
        }

        onDropped: {
            if (drag.source.type === "singleoperation" || drag.source.type === "workflow") {
                if (drag.source.type === "workflow") {
                    operations.refresh()
                }
                var onItem = itemAt(drag.x, drag.y)
                if (!onItem ){
                    addOperation(drag.source.ilwisobjectid,drag.x,drag.y)

                }else {
                    if ( onItem.type === "conditionitem"){
                        if (onItem.inOperationList(drag.y)){
                            var operation = addOperation(drag.source.ilwisobjectid,drag.x,drag.y)
                            if ( operation){
                                onItem.addToOperationList(currentItem)
                            }
                        }else {
                            onItem.addTestOperation(drag.source.ilwisobjectid)
                        }
                    }
                }
            }
        }
    }

    WFCanvas{
        id : wfCanvas
        anchors.top: tools.bottom
        width : parent.width
        height : parent.height - tools.height
    }

    Timer {
        interval: 50;
        running: true;
        repeat: true
        onTriggered: {
            if ( !wfCanvas.canvasValid)
                wfCanvas.draw(true)
        }
    }

    function drawFromWorkflow() {
        var nodes = workflow.getNodes()
        var kvp = []
        for(var i=0; i < nodes.length; ++i){
            var node = nodes[i];
            var operation = operations.operation(node.operationid)
            if ( !operation)
                return
            var component = Qt.createComponent("OperationItem.qml");
            if (component.status === Component.Ready){
                currentItem = component.createObject(wfCanvas, {"x": node.x, "y": node.y, "operation" : operation, "itemid" : node.nodeid, "scale": wfCanvas.scale});
                operationsList.push(currentItem)
                kvp[currentItem.itemid] = currentItem
            }
        }
        for(i=0; i < operationsList.length; ++i){
            var operationItem = operationsList[i]
            node = workflow.getNode(operationItem.itemid)
            if(!node)
                continue;

            var parameters = node.parameters
            for(var j=0; j < parameters.length; ++j){
                var parm = parameters[j]
                if ( parm.outputIndex !== -1){
                    var flowPoints = { "fromParameterIndex" :  j, "toParameterIndex" : parm.outputIndex};
                    var sourceItem = kvp[parm.outputNodeId]
                    var sourceRect = sourceItem.attachementRectangles[parm.sourceRect]
                    var targetRect = parm.targetRect
                    operationItem.flowConnections.push({
                                             "target" : operationItem,
                                             "source" :sourceItem,
                                             "attachtarget" : targetRect,
                                             "attachsource" : sourceRect,
                                             "flowPoints" : flowPoints,
                                             "isSelected" : false
                                         })
                }
            }

        }
        if ( workflowManager){
            workflowManager.updateRunForm()
        }
    }

    function showSelectedOperation(item){
        workflowManager.showSelectedOperation(item)
    }

    function updateInputNamesList() {
        var oper = currentOperation()
        if ( oper){
            oper.resetInputModel();
        }
    }

    function currentOperation() {
        if ( currentItem && currentItem.type === "operationitem"){
            return currentItem
        }
        return null
    }

    function zoom(amount, absolute, cx, cy){

        wfCanvas.oldZoomScale = wfCanvas.zoomScale;
        if ( absolute){
            wfCanvas.zoomScale = amount / 100.0

        }else if ( amount > 0){
            wfCanvas.zoomScale = Math.min(20,wfCanvas.zoomScale + Math.abs(amount)/100.0)
        }else{
            wfCanvas.zoomScale = Math.max(0.05,wfCanvas.zoomScale - Math.abs(amount)/100.0)
        }
        for(var i=0; i < operationsList.length; ++i){
            var dzoom = wfCanvas.zoomScale / wfCanvas.oldZoomScale
            var operation = operationsList[i]
            var dx = (operation.x - width / 2.0)
            var dy = (operation.y - height / 2.0)
            operation.x = width / 2.0 + dx * dzoom
            operation.y = height / 2.0 + dy *dzoom
        }
        for(i=0; i < conditionsList.length; ++i){
            dzoom = wfCanvas.zoomScale / wfCanvas.oldZoomScale
            var condition = conditionsList[i]
            dx = (condition.x - width / 2.0)
            dy = (condition.y - height / 2.0)
            condition.x = width / 2.0 + dx * dzoom
            condition.y = height / 2.0 + dy *dzoom
        }
        wfCanvas.canvasValid = false

    }

    function pan(px, py){
        for(var i=0; i < operationsList.length; ++i){
            var operation = operationsList[i]
            operation.x += px;
            operation.y += py;

        }
        for(i=0; i < conditionsList.length; ++i){
            var conditions = conditionsList[i]
            conditions.x += px;
            conditions.y += py;

        }
        wfCanvas.canvasValid = false
    }

    function itemAt(x,y){
        for(var i=0; i < operationsList.length; ++i){

            var item = operationsList[i]
            var startCoords = Qt.point(item.x, item.y)

            var endX = (startCoords.x + (item.width * item.scale));
            var endY = (startCoords.y + (item.height * item.scale));

            var EndCoords = Qt.point(endX, endY);

            if (x >= (startCoords.x) && y >= (startCoords.y) && x <= endX && y <= endY){
                return item
            }
        }

        for(i=0; i < conditionsList.length; ++i){

            item = conditionsList[i]
            startCoords = Qt.point(item.x, item.y)

            endX = (startCoords.x + (item.width * item.scale));
            endY = (startCoords.y + (item.height * item.scale));

            if (x >= (startCoords.x) && y >= (startCoords.y) && x <= endX && y <= endY){
                return item
            }
            for( var j=0; j < item.junctionsList.length; ++j){
                var junction = item.junctionsList[i]
                startCoords = Qt.point(junction.x, junction.y)

                endX = (startCoords.x + (junction.width * junction.scale));
                endY = (startCoords.y + (junction.height * junction.scale));

                if (x >= (startCoords.x) && y >= (startCoords.y) && x <= endX && y <= endY){
                    return junction
                }
            }
        }
        return null
    }

}
