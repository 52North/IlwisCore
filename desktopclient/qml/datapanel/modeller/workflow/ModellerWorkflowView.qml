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

        function addOperation(objectid,dx,dy,ownerid, parentItem){
            var operation = operations.operation(objectid)
            if ( operation){
                var nodeId
                if ( ownerid === -1)
                    nodeId = workflow.addNode(objectid,{x : dx, y:dy, w:240, h:130,type:'operationnode'})
                else
                    nodeId = workflow.addNode(objectid,{x : dx, y:dy, w:240, h:130,type:'operationnode',owner:ownerid,reason : "operations"})
                wfCanvas.createItem(dx, dy,operation, nodeId,parentItem)
            }
            if ( workflowManager){
                workflowManager.updateRunForm()
            }
            return operation
        }

        function addConnect(objectid,dx,dy,ownerid, parentItem){
            var connectid = operations.operationId("ilwis://operations/connect")
            addOperation(connectid,dx,dy,ownerid, parentItem)
            var object = mastercatalog.id2Resource(objectid, 0)
            if ( object){
                workflow.setFixedValues(currentItem.itemid, object.url)
                workflow.createMetadata()
                updateInputNamesList()
                object.suicide()
            }
        }

        onDropped: {
            if (drag.source.type === "singleoperation" || drag.source.type === "workflow") {
                if (drag.source.type === "workflow") {
                    operations.refresh()
                }
                 var onItem = itemAt(drag.x, drag.y)
                if (!onItem ){
                    addOperation(drag.source.ilwisobjectid,drag.x,drag.y,-1, wfCanvas)

                }else {
                    if ( onItem.type === "conditionitem"){
                        if (onItem.inOperationList(drag.y)){
                            var p = wfCanvas.mapToItem(onItem, drag.x, drag.y)
                            var operation = addOperation(drag.source.ilwisobjectid,p.x,p.y,onItem.itemid, onItem)
                            if ( operation){
                                onItem.addToOperationList(currentItem)
                            }
                        }else {
                            onItem.addTestOperation(drag.source.ilwisobjectid)
                        }
                    }
                }
            }else {
               addConnect(drag.source.ilwisobjectid,drag.x,drag.y,-1, wfCanvas)
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

    function getItem(nodeId){
        for(var i=0; i < operationsList.length; ++i){
            if ( operationsList[i].itemid == nodeId)
                return operationsList[i]
        }
        for(i=0; i < conditionsList.length; ++i){
            var condition = conditionsList[i]
            if ( condition.itemid == nodeId)
                return condition
            for(var j=0; j < condition.operationsList.length; ++j){
                if ( condition.operationsList[i].itemid === nodeId)
                    return condition.operationsList[i]
            }
            for(j=0; j < condition.junctionsList.length; ++j){
                if ( condition.junctionsList[i].itemid === nodeId)
                    return condition.junctionsList[i]
            }
        }
        return null
    }

    function createOperationItem(operationid,nodeid, px, py,parentitem){
        var operation = operations.operation(operationid)
        if ( !operation)
            return null
        var component = Qt.createComponent("OperationItem.qml");
        if (component.status === Component.Ready){
            var item = component.createObject(parentitem, {"x": px, "y": py, "operation" : operation, "itemid" : nodeid, "scale": wfCanvas.scale});
            operationsList.push(item)
            return item
        }
        return null
    }
    function recreateJunctionFlow(junctionItem, kvp){
        var node = workflow.getNode(junctionItem.itemid)
        if(!node)
            return

        var parm = node.linkedtrueoperation
            var flowPoints = { "fromParameterIndex" :  parm.outputIndex, "toParameterIndex" :1};
            var sourceItem = kvp[parm.outputNodeId]
            var sourceRect = sourceItem.attachementRectangles[parm.sourceRect]
            var targetRect = parm.targetRect
            sourceItem.flowConnections.push({
                                                   "target" : junctionItem,
                                                   "source" :sourceItem,
                                                   "attachtarget" : targetRect,
                                                   "attachsource" : sourceRect,
                                                   "flowPoints" : flowPoints,
                                                   "isSelected" : false
                                               })
        parm = node.linkedfalseoperation
        flowPoints = { "fromParameterIndex" :  parm.outputIndex, "toParameterIndex" :2};
        sourceItem = kvp[parm.outputNodeId]
        sourceRect = sourceItem.attachementRectangles[parm.sourceRect]
        targetRect = parm.targetRect
        sourceItem.flowConnections = []
        sourceItem.flowConnections.push({
                                               "target" : junctionItem,
                                               "source" :sourceItem,
                                               "attachtarget" : targetRect,
                                               "attachsource" : sourceRect,
                                               "flowPoints" : flowPoints,
                                               "isSelected" : false
                                           })
     }

    function recreateConditionFlow(conditionItem, kvp){
        var node = workflow.getNode(conditionItem.itemid)
        if(!node)
            return

        var tests = node.tests
        for(var i=0; i < tests.length; ++i){
            var testParameters = tests[i].operation.parameters

            for(var j=0; j < testParameters.length; ++j){
                recreateFlow(testParameters[j], kvp,conditionItem,j)
            }
        }
        for(var k=0; k < conditionItem.operationsList.length; ++k){
            recreateFlows(conditionItem.operationsList[k], kvp)
        }
    }

    function recreateFlow(parm,kvp,item, index){
        if ( parm && item && parm.outputIndex !== -1){
            var flowPoints = { "fromParameterIndex" :  index, "toParameterIndex" : parm.outputIndex};
            var sourceItem = kvp[parm.outputNodeId]
            var sourceRect = sourceItem.attachementRectangles[parm.sourceRect]
            var targetRect = parm.targetRect
            item.flowConnections.push({
                                     "target" : item,
                                     "source" :sourceItem,
                                     "attachtarget" : targetRect,
                                     "attachsource" : sourceRect,
                                     "flowPoints" : flowPoints,
                                     "isSelected" : false
                                 })
        }
    }

    function recreateFlows(operationItem, kvp) {
        var node = workflow.getNode(operationItem.itemid)
        if(!node)
            return

        var parameters = node.parameters
        for(var j=0; j < parameters.length; ++j){
            recreateFlow(parameters[j], kvp,operationItem,j)
        }
    }

    function recreateWorkflow() {

        var nodes = workflow.getNodes()
        var kvp = []
        var unlinkedJunctions = []
        for(var i=0; i < nodes.length; ++i){
            var node = nodes[i];
            if ( node.type === "conditionnode"){
                var component = Qt.createComponent("ConditionItem.qml");
                if (component.status === Component.Ready){
                    currentItem = component.createObject(wfCanvas, {"x": node.x, "y": node.y, "height" : node.h, "width" : node.w,"itemid" : node.nodeid, "scale": wfCanvas.scale});
                    workarea.conditionsList.push(currentItem)
                    currentItem.setTests()
                    kvp[currentItem.itemid] = currentItem
                    var ownedoperations = node.ownedoperations;
                   // recreateConditionFlow(currentItem, kvp)
                    for(var j = 0;  j < ownedoperations.length; ++j){
                        var oper = ownedoperations[j]
                        var item = createOperationItem(oper.operationid, oper.nodeid,oper.x, oper.y,currentItem)
                        if ( item){
                            kvp[item.itemid] = item
                            item.condition = currentItem
                            currentItem.operationsList.push(item)
                        }
                    }

                }
            }else if ( node.type === "operationnode"){
                currentItem = createOperationItem(node.operationid,node.nodeid,node.x, node.y, wfCanvas)
                if ( currentItem)
                    kvp[currentItem.itemid] = currentItem
            }else if ( node.type === "junctionnode"){
                var comp = Qt.createComponent("JunctionItem.qml");
                if (comp.status === Component.Ready){
                    currentItem = comp.createObject(wfCanvas, {"x": node.x, "y": node.y, "height" : node.h, "width" : node.w,"itemid" : node.nodeid, "scale": wfCanvas.scale});
                    kvp[currentItem.itemid] = currentItem
                    unlinkedJunctions.push({"condition": node.linkedcondition, "junction" : currentItem})
                }
            }
        }
        for(i=0; i < operationsList.length; ++i){
            var operationItem = operationsList[i]
            recreateFlows(operationItem, kvp)

        }

        for(i=0; i < conditionsList.length; ++i){
            var conditionItem = conditionsList[i]
            recreateConditionFlow(conditionItem, kvp)


        }

        for(i=0; i < unlinkedJunctions.length;++i){
             conditionItem  = getItem(unlinkedJunctions[i].condition)
                var junction = unlinkedJunctions[i].junction
                if ( junction){
                    conditionItem.junctionsList.push(junction)
                    junction.linkedCondition = conditionItem
                }
                recreateJunctionFlow(junction, kvp)
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
         var op = operationsList[0]
        var dz = wfCanvas.zoomScale /wfCanvas.oldZoomScale

        wfCanvas.oldZoomScale = wfCanvas.zoomScale;
        if ( absolute){
            wfCanvas.zoomScale = amount / 100.0

        }else if ( amount > 0){
            wfCanvas.zoomScale = Math.min(20,wfCanvas.zoomScale + Math.abs(amount)/100.0)
        }else{
            wfCanvas.zoomScale = Math.max(0.05,wfCanvas.zoomScale - Math.abs(amount)/100.0)
        }
        var dzoom = wfCanvas.zoomScale /wfCanvas.oldZoomScale
        for(var i=0; i < operationsList.length; ++i){
            var operation = operationsList[i]
            if ( !operation.condition){
                zoomItem(operation, dzoom)
            }
        }
        for(i=0; i < conditionsList.length; ++i){

            var condition = conditionsList[i]
            zoomItem(condition, dzoom)
            for(var j=0; j < condition.junctionsList.length; ++j){
                zoomItem(condition.junctionsList[j], dzoom)
            }
        }
        wfCanvas.canvasValid = false

    }

    function zoomItem(item, dzoom,cx,cy){
        item.x = item.x * dzoom - (item.width * (1.0 - dzoom))/2.0
        item.y = item.y * dzoom - (item.height * (1.0 - dzoom))/2.0
    }

    function pan(px, py){
        for(var i=0; i < operationsList.length; ++i){
            var operation = operationsList[i]
            if ( operation.condition)
                continue;
            operation.x -= px;
            operation.y -= py;

        }
        for(i=0; i < conditionsList.length; ++i){
            var conditions = conditionsList[i]
            conditions.x -= px;
            conditions.y -= py;
            for(var j=0; j < conditions.junctionsList.length;++j){
                conditions.junctionsList[j].x -= px;
                conditions.junctionsList[j].y -= py;
            }

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

            for( var j=0; j < item.operationsList.length; ++j){
                var op = item.operationsList[j]
                var  p = item.mapToItem(wfCanvas, op.x, op.y)
                startCoords = Qt.point(p.x, p.y)

                endX = (startCoords.x + (op.width * op.scale));
                endY = (startCoords.y + (op.height * op.scale));

                if (x >= (startCoords.x) && y >= (startCoords.y) && x <= endX && y <= endY){
                    return op
                }
            }
            startCoords = Qt.point(item.x, item.y)
            endX = (startCoords.x + (item.width * item.scale));
            endY = (startCoords.y + (item.height * item.scale));

            if (x >= (startCoords.x) && y >= (startCoords.y) && x <= endX && y <= endY){
                return item
            }

            for( var j=0; j < item.junctionsList.length; ++j){
                var junction = item.junctionsList[j]
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
