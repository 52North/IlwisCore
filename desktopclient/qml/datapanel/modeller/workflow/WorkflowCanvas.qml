import QtQuick 2.0;
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import WorkflowModel 1.0
import QtQuick.Dialogs 1.1
import ".." as Modeller


Modeller.ModellerWorkArea {
    property WorkflowModel workflow;
    property OperationCatalogModel operationCatalog;
    property var deleteItemIndex;
    property var deleteEdgeIndex;

    function asignConstantInputData(inputData, itemId) {
        workflow.asignConstantInputData(inputData, itemId)
        wfCanvas.operationsList[itemId].resetInputModel()
    }

    function deleteSelectedOperation(){
        for(var i=0; i < wfCanvas.operationsList.length; ++i){
            var item = wfCanvas.operationsList[i]
            if (item.isSelected) {
                deleteItemIndex = i
                deleteOperationDialog.open()
                break
            }
        }
    }

    function deleteSelectedEdge(){
        var flow = getSelectedEdge()
        if(flow != 0)
        {
            messageDialogEdge.open()
        }
    }

    function alterSelectedEdge(){
        var flow = getSelectedEdge()
        if(flow != 0)
        {
            //Retrieve target and rectangle before deleting the edge
            var target = flow.target;
            var attachedRect = flow.attachtarget;

            //Delete the edge
            var from = flow.source.itemid
            var to = flow.target.itemid
            var inputIndex = flow.flowPoints.toParameterIndex
            var outputIndex = flow.flowPoints.fromParameterIndex

            workflow.deleteFlow(from, to, outputIndex, inputIndex)
            wfCanvas.operationsList[deleteItemIndex].flowConnections.splice(deleteEdgeIndex, 1)
            flow.target.resetInputModel()
            wfCanvas.canvasValid = false

            //Create a new edge
            wfCanvas.showAttachmentForm(true, flow.target, flow.attachtarget);
        }
    }

    function getSelectedEdge(){
        for(var i=0; i < wfCanvas.operationsList.length; ++i){
            var item = wfCanvas.operationsList[i]

            for(var j=0; j < item.flowConnections.length; j++)
            {
                var flow = item.flowConnections[j];

                if(flow.isSelected)
                {
                    deleteItemIndex = i;
                    deleteEdgeIndex = j;
                    return flow;
                }
            }
        }
        return 0;
    }

    MessageDialog {
        id: deleteOperationDialog
        title: "Deleting operation"
        text: "Are you sure you want to delete this operation?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            var item = wfCanvas.operationsList[deleteItemIndex]
            var flows = item.flowConnections;

            // First delete the operation at C++. THIS NEEDS TO BE DONE FIRST
            workflow.deleteOperation(deleteItemIndex)

            // This removes 1 (second param) from the operation list beginning from deleteItemIndex
            wfCanvas.operationsList.splice(deleteItemIndex, 1)

            // Delete incomming connections of the operation which will be deleted
            for (var i = 0; i < wfCanvas.operationsList.length; i++) {
                var operation = wfCanvas.operationsList[i]
                var deleteFlows = [];
                // Search for deletable connections
                for (var j = 0; j < operation.flowConnections.length; j++) {
                    var flow = operation.flowConnections[j]
                    // If target is same as deleted operation
                    if (flow.target.itemid == item.itemid) {
                        deleteFlows.push(j)
                    }
                }
                // Delete the connections
                for (var j = 0; j < deleteFlows.length; j++) {
                    var deleteIndex = deleteFlows[j];
                    wfCanvas.operationsList[i].flowConnections.splice(deleteIndex, 1)
                }
            }
            // Loop through all operations after the deleted item. We need to reset their itemid
            for (var i = deleteItemIndex; i < wfCanvas.operationsList.length; i++) {
                operation.itemid = i
            }

            // Destroy the QML object.
            item.destroy()

            // Reset all targets of the flows of the deleted operation
            for (var i = 0; i < flows.length; i++) {
                flows[i].target.resetInputModel()
            }

            // Redraw lines
            wfCanvas.canvasValid = false
            wfCanvas.draw(true)
        }
        Component.onCompleted: visible = false
    }

    MessageDialog {
        id: messageDialogEdge
        title: "Deleting edge"
        text: "Are you sure you want to delete this edge?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            var flow = wfCanvas.operationsList[deleteItemIndex].flowConnections[deleteEdgeIndex]
            var from = flow.source.itemid
            var to = flow.target.itemid
            var inputIndex = flow.flowPoints.toParameterIndex
            var outputIndex = flow.flowPoints.fromParameterIndex

            workflow.deleteFlow(from, to, outputIndex, inputIndex)
            wfCanvas.operationsList[deleteItemIndex].flowConnections.splice(deleteEdgeIndex, 1)
            flow.target.resetInputModel()
            wfCanvas.canvasValid = false
        }
        Component.onCompleted: {
            visible = false
        }
    }

    /**
      Calls the WorkflowModel's run method
      */
    function run(){
        workflow.createMetadata()
        manager.retrieveRunFormValues()
    }

    /**
      Calls the create meta data method of the WorkflowModel and regenerates the form
      */
    function generateForm() {
        if (workflow){
            workflow.createMetadata()
            manager.showRunForm(workflow.id)
        }
    }

    /**
      Draws canvas from the workflow
      */
    function drawFromWorkflow() {
        var nodes = workflow.getNodes(), node, resource, edges, edge, fromItemid,
                toItemId, fromOperation=false, toOperation=false, flowPoints;
        for (var i = 0; i < nodes.length; i++) {
            node = nodes[i];
            resource = mastercatalog.id2Resource(node.operationId)

            wfCanvas.createItem(node.x, node.y, resource);
            edges = workflow.getEdges(node)
            for (var j = 0; j < edges.length; j++) {
                edge = edges[j]
                fromItemid = workflow.vertex2ItemID(node.vertex) //TODO: Temporary
                toItemId = workflow.vertex2ItemID(edge.toVertex) //TODO: Temporary

                for (var k = 0; k < wfCanvas.operationsList.length; k++) {
                    if (wfCanvas.operationsList[k].itemid == fromItemid && !fromOperation) {
                        fromOperation = wfCanvas.operationsList[k]
                    } else if (wfCanvas.operationsList[k].itemid == toItemId && !toOperation) {
                        toOperation = wfCanvas.operationsList[k]
                    }
                }

                if (fromOperation && toOperation){
                    flowPoints = {
                        "fromParameterIndex" : edge.fromParameter,
                        "toParameterIndex" : edge.toParameter
                    }
                    fromOperation.flowConnections.push({
                       "target" : toOperation,
                       "source" : fromOperation,
                       "attachtarget" : toOperation.index2Rectangle(edge.toRect),
                       "attachsource" : fromOperation.index2Rectangle(edge.fromRect),
                       "flowPoints" : flowPoints,
                       "isSelected" : false
                    })
                }

                fromOperation = false
                toOperation = false
            }
        }
        wfCanvas.draw(true)
    }

    Canvas {
        Keys.onDeletePressed: {
            deleteSelectedOperation()
        }
        Keys.onBackPressed: {
            deleteSelectedOperation()
        }

        id : wfCanvas
        anchors.fill: parent

        property var ctx: getContext('2d')
        property bool canvasValid: true

        property double oldx : -1.0
        property double oldy : -1.0
        property point workingLineBegin : Qt.point(-1,-1)
        property point workingLineEnd : Qt.point(-1,-1)
        property int currentIndex : 0
        property var component;
        property var currentItem;
        property var operationsList: []

        Timer {
            interval: 30;
            running: true;
            repeat: true
            onTriggered: {
                wfCanvas.draw()
            }
        }


        /*
     * Get the resource by id from MasterCatalog
     */
        function getOperation(id) {
            var oper = operations.operation(id);
            return oper;
        }

        function invalidate() {
            canvasValid = false;
        }

        function draw(force){
            if (canvasValid == false || (force !== null && force)) {
                clear();
                canvasValid = true
                if ( workingLineBegin.x !== -1 && workingLineEnd.x !== -1){
                    ctx.beginPath();
                    ctx.lineWidth = 2;
                    ctx.moveTo(workingLineBegin.x, workingLineBegin.y);
                    ctx.strokeStyle = "red"
                    ctx.lineTo(workingLineEnd.x, workingLineEnd.y);
                    ctx.stroke();
                }
                for( var i=0; i < operationsList.length; i++){
                    operationsList[i].drawFlows(ctx)
                }
                //wfCanvas.requestPaint();
                generateForm()
            }
        }

        function createItem(x,y, resource) {
            component = Qt.createComponent("OperationItem.qml");

            if (component.status == Component.Ready)
                finishCreation(x,y,resource);
            else
                component.statusChanged.connect(finishCreation);
        }

        function finishCreation(x,y,resource) {
            if (component.status == Component.Ready) {
                currentItem = component.createObject(wfCanvas, {"x": x, "y": y, "operation" : resource,"itemid" : operationsList.length});
                if (currentItem == null) {
                    // Error Handling
                    console.log("Error creating object");
                }
                operationsList.push(currentItem)

            } else if (component.status == Component.Error) {
                // Error Handling
                console.log("Error loading component:", component.errorString());
            }
        }

        /*
     * Clear the Canvas
     */
        function clear() {
            if (ctx){
                ctx.reset();
                ctx.clearRect(0, 0, width, height);
                ctx.stroke();
                wfCanvas.requestPaint();
            }
        }

        function stopWorkingLine(){
            wfCanvas.oldx = -1.0
            wfCanvas.oldy = -1.0
            wfCanvas.workingLineBegin = Qt.point(-1,-1)
            wfCanvas.workingLineEnd = Qt.point(-1,-1)
            wfCanvas.canvasValid = true
        }

        function showAttachmentForm(yesno, target, attachRect){
            var fromOperation = operationsList[wfCanvas.currentIndex].operation
            attachementForm.operationFrom = fromOperation
            attachementForm.operationTo = target.operation
            attachementForm.attachRect = attachRect
            attachementForm.target = target
            attachementForm.state = yesno ? "visible" : "visible"
        }

        onWidthChanged: {
            // force re-draw if the ModellerPanel width has changed
            invalidate();
        }

        onHeightChanged: {
            // force re-draw if the ModellerPanel height has changed
            wfCanvas.draw(true);
        }

        DropArea {
            id: canvasDropArea
            anchors.fill: wfCanvas
            onDropped: {
                if (drag.source.type === "singleoperation" || drag.source.type === "workflow") {
                    var oper = wfCanvas.getOperation(drag.source.ilwisobjectid)
                    wfCanvas.createItem(drag.x - 50, drag.y - 30,oper)
                    workflow.addOperation(drag.source.ilwisobjectid)

                    generateForm()
                }
            }
        }
        FlowParametersChoiceForm{
            id : attachementForm
        }

        MouseArea {
            id: area
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            hoverEnabled: wfCanvas.workingLineBegin.x !== -1

            onPressed: {
                wfCanvas.canvasValid = false;

                var selected = false, pressed = -1;

                for(var i=0; i < wfCanvas.operationsList.length; ++i){

                    var item = wfCanvas.operationsList[i]
                    var isContained = mouseX >= item.x && mouseY >= item.y && mouseX <= (item.x + item.width) && mouseY <= (item.y + item.height)

                    for(var j=0; j < item.flowConnections.length; j++)
                    {
                        var flow = item.flowConnections[j];

                        // Retrieve basic X and Y positions of the line
                        var startPoint = flow.attachsource.center();
                        var endPoint = flow.attachtarget.center();
                        var ax = startPoint.x;
                        var ay = startPoint.y;
                        var bx = endPoint.x;
                        var by = endPoint.y;

                        // Calculate distance to check mouse hits a line
                        var distanceAC = Math.sqrt(Math.pow((ax-mouseX), 2) + Math.pow((ay-mouseY), 2));
                        var distanceBC = Math.sqrt(Math.pow((bx-mouseX), 2) + Math.pow((by-mouseY), 2));
                        var distanceAB = Math.sqrt(Math.pow((ax-bx), 2) + Math.pow((ay-by), 2));


                        // Check if mouse intersects the line with offset of 10
                        if(!selected && (distanceAC + distanceBC) >= distanceAB &&
                           (distanceAC + distanceBC) < (distanceAB + 10))
                        {
                            selected = true;
                            flow.isSelected = true;
                        } else {
                            flow.isSelected = false;
                        }
                    }

                    if ( isContained) {
                        pressed = i
                    }
                    item.isSelected = false
                }
                wfCanvas.oldx = mouseX
                wfCanvas.oldy = mouseY
                if (pressed > -1) {
                    wfCanvas.currentIndex = pressed;
                    item = wfCanvas.operationsList[pressed]
                    item.isSelected = true

                    var definedValueIndexes = workflow.definedValueIndexes(pressed)

                    if(definedValueIndexes){
                        manager.showOperationFormWithHiddenFields(item.operation.id,pressed, definedValueIndexes)
                    }else{
                        manager.showOperationForm(item.operation.id, pressed)
                    }

                    manager.showMetaData(item.operation)
                } else {
                    manager.resetMetaData(workflow);
                }


            }

            onDoubleClicked: {
                var pressed = -1, item = 0;
                for(var i=0; i < wfCanvas.operationsList.length; ++i){

                    item = wfCanvas.operationsList[i]
                    var isContained = mouseX >= item.x && mouseY >= item.y && mouseX <= (item.x + item.width) && mouseY <= (item.y + item.height)

                    if ( isContained) {
                        pressed = i
                    }
                }
                if (pressed > -1) {
                    var resource = mastercatalog.id2Resource(item.operation.id)
                    var filter = "itemid=" + resource.id
                    bigthing.newCatalog(filter, "workflow",resource.url,"other")
                }
            }

            Keys.onEscapePressed: {
                console.log("escape key");
                wfCanvas.stopWorkingLine()
            }

            onPositionChanged: {
                if ( attachementForm.state == "invisible"){
                    if ( wfCanvas.workingLineBegin.x !== -1){
                        wfCanvas.workingLineEnd = Qt.point( mouseX, mouseY)
                        wfCanvas.canvasValid = false
                    }
                    if ( wfCanvas.oldx >= 0 && wfCanvas.oldy >= 0 && wfCanvas.currentIndex >= 0)    {

                        var item = wfCanvas.operationsList[wfCanvas.currentIndex]
                        if ( item){
                            item.x += ( mouseX - wfCanvas.oldx)
                            item.y += (mouseY - wfCanvas.oldy)
                            wfCanvas.oldx = mouseX
                            wfCanvas.oldy = mouseY
                        }
                    }
                }
            }

            onReleased: {
                wfCanvas.stopWorkingLine()
            }
        }


    }
}
