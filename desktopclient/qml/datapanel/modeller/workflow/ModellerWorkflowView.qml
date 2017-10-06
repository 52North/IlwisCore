import QtQuick 2.1
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import WorkflowModel 1.0
import QtQuick.Dialogs 1.1
import ".." as Modeller
import "../../../matrix.js" as Matrix
import "../../../Global.js" as Global
import "../../../controls" as Controls

Modeller.ModellerWorkArea {
    id : workarea
    property WorkflowModel workflow;
    property var workflowManager
    //property OperationCatalogModel operationCatalog;
    property var operationsList: []
    property var conditionsList: []
    property var rangesList: []

    property var currentItem
    property bool selectState : false
    property bool canvasActive : true;
    property bool dropCondition : false
    property bool dropLoop : false

    onDropConditionChanged: {
        tools.dropLoop(false)
        tools.dropCondition(dropCondition)
    }

    onDropLoopChanged: {
        tools.dropCondition(false)
        tools.dropLoop(dropLoop)
    }

    state : "visible"

    WorkflowTools{
        id : tools
        z : 10

    }
    SaveAsBox {
        id : savestuff
        anchors.top : tools.bottom
        anchors.topMargin: 2
        state : "invisible"
        clip:true
        z : 1000
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
            var operation = addOperation(connectid,dx,dy,ownerid, parentItem)
            var object = mastercatalog.id2Resource(objectid, 0)
            if ( object && operation){
                workflow.setFixedValues(operation.itemid, object.url)
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
                    if ( onItem.type === "conditionitem" || onItem.type === "rangeitem"){
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
                onItem = itemAt(drag.x, drag.y)
                if ( onItem && onItem.type === "rangeitem"){
                    if ( onItem.droppedOnTestPart( drag.x, drag.y)){
                        onItem.addTestDataObject(drag.source.ilwisobjectid, drag.source.message, drag.source.type)
                        return
                    }
                }

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
        for(i=0; i < rangesList.length; ++i){
            var range = rangesList[i]
            if ( range.itemid == nodeId)
                return range
            for(var j=0; j < range.operationsList.length; ++j){
                if ( range.operationsList[i].itemid === nodeId)
                    return range.operationsList[i]
            }
            for(j=0; j < range.junctionsList.length; ++j){
                if ( range.junctionsList[i].itemid === nodeId)
                    return range.junctionsList[i]
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

    function createRangeJunctionItem(nodeid, px, py,parentitem){
        var component = Qt.createComponent("RangeJunctionItem.qml");
        if (component.status === Component.Ready){
            var item = component.createObject(parentitem, {"x": px, "y": py, "itemid" : nodeid, "scale": wfCanvas.scale});
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
        var sourceItem = kvp[parm.outputNodeId] // need not to exist in case incomplete flows are saved
        if ( sourceItem){
            var sourceRect = sourceItem.attachementRectangles[parm.sourceRect]
            var targetRect = parm.targetRect
            sourceItem.addFlowConnection(junctionItem, sourceItem, targetRect, sourceRect, flowPoints, -1,-1)
        }

        parm = node.linkedfalseoperation
        flowPoints = { "fromParameterIndex" :  parm.outputIndex, "toParameterIndex" : 2};
        sourceItem = kvp[parm.outputNodeId]
        if ( sourceItem){ // need not to exist in case incomplete flows are saved
            sourceRect = sourceItem.attachementRectangles[parm.sourceRect]
            targetRect = parm.targetRect
            // sourceItem.flowConnections = []
            sourceItem.addFlowConnection(junctionItem, sourceItem, targetRect, sourceRect, flowPoints, -1,-1)
        }
    }

    function recreateConditionFlow(conditionItem, kvp, op){
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
            //console.debug("from", index, "to", parm.outputIndex)
            var flowPoints = { "fromParameterIndex" :  parm.outputIndex, "toParameterIndex" : index};
            var sourceItem = kvp[parm.outputNodeId]
            var sourceRect = sourceItem.attachementRectangles[parm.sourceRect]
            var targetRect = parm.targetRect
            sourceItem.addFlowConnection(item, sourceItem, targetRect, sourceRect, flowPoints, -1,-1)
       }
    }

    function recreateFlows(operationItem, kvp) {
        var node = workflow.getNode(operationItem.itemid)
        if(!node)
            return

        var parameters = node.parameters
        if (!parameters)
            return

        for(var j=0; j < parameters.length; ++j){
            recreateFlow(parameters[j], kvp,operationItem,j)
        }
    }

    function recreateRangeFlows(rangeItem, kvp) {
        var node = workflow.getNode(rangeItem.itemid)
        if(!node)
            return

        var parameters = node.parameters
        if (!parameters)
            return

        for(var j=0; j < parameters.length; ++j){
            recreateFlow(parameters[j], kvp,rangeItem,j)
        }

        for(var k=0; k < rangeItem.operationsList.length; ++k){
            recreateFlows(rangeItem.operationsList[k], kvp)
        }
        for( k=0; k < rangeItem.junctionsList.length; ++k){
            recreateFlows(rangeItem.junctionsList[k], kvp)
        }
    }

    function recreateWorkflow() {
        if (!workflow)
            return
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
            } else if ( node.type === "rangenode"){
                component = Qt.createComponent("RangeItem.qml");
                if (component.status === Component.Ready){
                    currentItem = component.createObject(wfCanvas, {"x": node.x, "y": node.y, "height" : node.h, "width" : node.w,"itemid" : node.nodeid, "scale": wfCanvas.scale});
                    workarea.rangesList.push(currentItem)
                    currentItem.setTests()
                    kvp[currentItem.itemid] = currentItem
                    ownedoperations = node.ownedoperations;
                    for(j = 0;  j < ownedoperations.length; ++j){
                        oper = ownedoperations[j]
                        item = createOperationItem(oper.operationid, oper.nodeid,oper.x, oper.y,currentItem)
                        if ( item){
                            kvp[item.itemid] = item
                            item.range = currentItem
                            currentItem.operationsList.push(item)
                        }
                    }
                    var ownedjunctions = node.ownedjunctions;
                    for(j = 0;  j < ownedjunctions.length; ++j){
                        var junction = ownedjunctions[j]
                        item = createRangeJunctionItem(junction.nodeid,junction.x, junction.y,currentItem)
                        if ( item){
                            kvp[item.itemid] = item
                            item.condition = currentItem
                            currentItem.junctionsList.push(item)
                        }
                    }
                }
            }else if ( node.type === "operationnode"){
                currentItem = createOperationItem(node.operationid,node.nodeid,node.x, node.y, wfCanvas)
                if ( currentItem){
                    kvp[currentItem.itemid] = currentItem
                    var collapsed = workflow.collapsed(node.nodeid)
                    currentItem.state = collapsed ? "minimized" : "maximized"
                }
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

        for(i=0; i < rangesList.length; ++i){
            var rangeItem = rangesList[i]
            recreateRangeFlows(rangeItem, kvp)
        }

        for(i=0; i < unlinkedJunctions.length;++i){
            conditionItem  = getItem(unlinkedJunctions[i].condition)
            var junction = unlinkedJunctions[i].junction
            if ( junction){

                conditionItem.junctionsList.push(junction)
                junction.condition = conditionItem
            }
            recreateJunctionFlow(junction, kvp)
        }
        wfCanvas.zoomScale = workflow.scale
        zoom(workflow.scale * 100,true,-1,-1)
        var xytranslation = workflow.translation
        pan(-xytranslation["x"], -xytranslation["y"])

        if ( workflowManager){
            workflowManager.updateRunForm()
        }
        currentItem = null // nothing is current now
    }

    function showSelectedOperation(item){
        workflowManager.showSelectedOperation(item)
    }

    function updateMetadata() {
        workflowManager.updateMetadata()
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
        if ( cx === -1 || cy === -1){
            cx = width / 2.0
            cy = height / 2.0
        }
        if ( absolute){
            wfCanvas.zoomScale = amount / 100.0
        }else
            wfCanvas.zoomScale = Math.min(20,wfCanvas.zoomScale + amount/100.0)

        var dzoom = wfCanvas.zoomScale /wfCanvas.oldZoomScale
        for(var i=0; i < operationsList.length; ++i){
            var operation = operationsList[i]
            if ( !operation.condition &&  !operation.range){
                zoomItem(operation, dzoom, cx,cy)
            }
        }
        for(i=0; i < conditionsList.length; ++i){

            var condition = conditionsList[i]
            zoomItem(condition, dzoom, cx,cy)
            for(var j=0; j < condition.junctionsList.length; ++j){
                zoomItem(condition.junctionsList[j], dzoom, cx,cy)
            }
        }
        for(i=0; i < rangesList.length; ++i){

            var range = rangesList[i]
            zoomItem(range, dzoom, cx,cy)
        }
        wfCanvas.canvasValid = false
        var num = Math.round(wfCanvas.zoomScale * 100)
        tools.setZoomEdit(num + "%")
        workflow.scale = wfCanvas.zoomScale

    }

    function zoomItem(item, dzoom, cx,cy){
        item.x = dzoom *(item.x - cx) + cx
        item.y = dzoom * (item.y - cy) + cy
        item.scale  = wfCanvas.zoomScale
    }

    function pan2(amount, direction){
        if ( direction === "up"){
            pan(0, amount)
        }
        if ( direction === "down"){
            pan(0, -amount)
        }
        if ( direction === "right"){
            pan(-amount, 0)
        }
        if ( direction === "left"){
            pan(amount, 0)
        }
    }

    function pan(px, py){
        for(var i=0; i < operationsList.length; ++i){
            var operation = operationsList[i]
            if ( operation.condition || operation.range)
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
        for(i=0; i < rangesList.length; ++i){
            var range = rangesList[i]
            range.x -= px;
            range.y -= py;
        }
        workflow.translateObject(px,py,true)
        wfCanvas.canvasValid = false
    }

    function checkContainer(x,y,nodeList, container){
        for( var j=0; j < nodeList.length; ++j){
            var node = nodeList[j]
          //  var inContainer = node.condition !== null || node.range !== null
         //   if ( !inContainer && container !== null) // any node that is contained can be ignored when running for the outer area
         //       continue;
            var startCoords = Qt.point(node.x, node.y)
            if ( container){
                var  p = wfCanvas.mapToItem(container, x, y)
                var endX = (node.x + (node.width * node.scale));
                var endY = (node.y + (node.height * node.scale));

                if (p.x >= (node.x) && p.y >= (node.y) && p.x <= endX && p.y <= endY){
                    return node
                }

            }else {
                if ( node.type === "operationitem"){
                    if ( node.condition === null && node.range === null ){
                        endX = (startCoords.x + (node.width * node.scale));
                        endY = (startCoords.y + (node.height * node.scale));
                        if (x >= (startCoords.x) && y >= (startCoords.y) && x <= endX && y <= endY){
                            return node
                        }
                    }
                }else   if ( node.type === "junctionitem"){
                    endX = (startCoords.x + (node.width * node.scale));
                    endY = (startCoords.y + (node.height * node.scale));
                    if (x >= (startCoords.x) && y >= (startCoords.y) && x <= endX && y <= endY){
                        return node
                    }
                }

            }
        }
        if ( container){
            startCoords = Qt.point(container.x, container.y)
            endX = (startCoords.x + (container.width * container.scale));
            endY = (startCoords.y + (container.height * container.scale));

            if (x >= (startCoords.x) && y >= (startCoords.y) && x <= endX && y <= endY){
                return container
            }
        }
        return null
    }

    function itemAt(x,y){
        var item = checkContainer(x,y,operationsList, null)

        if (item){
            return item;
        }
        for(var i=0; i < conditionsList.length; ++i){
            var condition = conditionsList[i]
            item = checkContainer(x,y,condition.operationsList, condition)
            if ( item){
                return item
            }
            item = checkContainer(x,y, condition.junctionsList, null)
            if ( item){
                return item
            }
        }
        for(i=0; i < rangesList.length; ++i){
            var range = rangesList[i]
            item = checkContainer(x,y,range.operationsList, range)
            if ( item){
                if ( item.type === "operationitem")
                    return item
                var internalnode = checkContainer(x,y, range.junctionsList, range)
                if ( internalnode){
                    return internalnode
                }
                return item
            }
        }
        return null
    }

    function removeItemFromList(list, nodeid){
        var item
        for(var i=0; i < list.length; ++i)    {
            if ( list[i].itemid == nodeid){
                item = list[i]
                list.splice(i,1)
                break;
            }
        }
        return item
    }

    function removeLinkTo(nodeid){
        for(var j=0; j < operationsList.length; ++j){
            operationsList[j].removeLinkTo(nodeid)
        }
        for(j=0; j < conditionsList.length; ++j){
            conditionsList[j].removeLinkTo(nodeid)
        }
        for(j=0; j < rangesList.length; ++j){
            rangesList[j].removeLinkTo(nodeid)
        }
    }

    function deleteSelectedItem(){
        if ( currentItem){
            if ( currentItem.type !== "flowconnection"){
                workflow.removeNode(currentItem.itemid)
                var item
                if ( currentItem.type === "operationitem"){
                    item = removeItemFromList(operationsList, currentItem.itemid)
                    removeLinkTo(currentItem.itemid)

                }else if ( currentItem.type === "conditionitem"){
                    item = removeItemFromList(conditionsList, currentItem.itemid)
                    removeLinkTo(currentItem.itemid)
                    item.removeContent()
                }else if ( currentItem.type === "rangeitem"){
                    item = removeItemFromList(rangesList, currentItem.itemid)
                    removeLinkTo(currentItem.itemid)
                    item.removeContent()
                }else if ( currentItem.type === "rangejunctionitem"){
                    console.debug(currentItem)
                    item = removeItemFromList(currentItem.condition.junctionsList, currentItem.itemid)
                    removeLinkTo(currentItem.itemid)
                }


                if ( item){
                    item.destroy()
                    currentItem = null
                    wfCanvas.canvasValid = false
                }
            }else if ( currentItem.type === "flowconnection"){
                workflow.deleteFlow(currentItem.target.itemid, currentItem.flowPoints.toParameterIndex)
                console.debug(currentItem, currentItem.source, currentItem.target,currentItem.target.itemid)
                currentItem.source.removeLinkTo(currentItem.target.itemid)
                wfCanvas.canvasValid = false
            }
        }

    }
    function dropSaveBox(xpos) {
        savestuff.x = xpos
        savestuff.toggle()


    }
    function refreshCanvas() {
        wfCanvas.canvasValid = false
    }

    function addFlowConnection(connections, targetItem, sourceItem, attachRectIndex,attachSource, flowPoints, testIndex, testParameter){
        for(var i=0; i < connections.length; ++i){
            var flow = connections[i]
            if ( flow.target.itemid === targetItem.itemid && flow.source.itemid === sourceItem.itemid){
                if ( flow.flowPoints.fromParameterIndex === flowPoints.fromParameterIndex &&
                     flow.flowPoints.toParameterIndex === flowPoints.toParameterIndex   ){
                    return
                }
            }
        }
        connections.push({
                                 "target" : targetItem,
                                 "source" :sourceItem,
                                 "attachtarget" : attachRectIndex,
                                 "attachsource" : attachSource,
                                 "flowPoints" : flowPoints,
                                 "isSelected" : false,
                                 "testindex" : testIndex,
                                 "testparameter" : testParameter,
                                 "type" : "flowconnection"
                             })
    }
    function nextStepWithList(list) {
        for(var i=0; i < list.length; ++i)    {
            if ( list[i].itemid == workflow.lastOperationNode){
                list[i].background = list[i].getBackground(workflow.lastOperationNode)
            }else{
                var item = list[i]
                if ( item.type === "conditionitem"){
                    nextStepWithList(item.operationsList)
                }
                list[i].background = list[i].getBackground(-1)
            }
        }
    }

    function nextStep(){
        nextStepWithList(operationsList)
    }

    function collapseWithList(yesno, list){
        for(var i=0; i < list.length; ++i)    {
            list[i].state = yesno ? "minimized" : "maximized"
            workflow.collapsed(list[i].itemid, list[i].state === "minimized")
            if ( item.type === "conditionitem"){
                nextStepWithList(yesno, item.operationsList)
            }
            if ( item.type === "rangeitem"){
                nextStepWithList(yesno, item.operationsList)
            }
        }
    }

    function collapse(yesno){
        collapseWithList(yesno, operationsList)
    }

    function clearCurrent() {
        if ( !workflow)
            return

        currentItem = null
        dropCondition = false
        dropLoop = false

        for(var i=0; i < operationsList.length; ++i){
            operationsList[i].destroy()
        }
        operationsList = []
        for(i=0; i < conditionsList.length; ++i){
            var item = conditionsList[i]
            item.clear()
        }
        conditionsList = []
        for(i=0; i < rangesList.length; ++i){
            item = rangesList[i]
            item.clear()
        }
        rangesList = []
         workflow = null
        wfCanvas.canvasValid = false
    }

    function storeRangeDefinitions() {
        for(var i=0; i < rangesList.length; ++i)    {
            rangesList[i].storeRangeDefinition();
        }
    }

}
