import QtQuick 2.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import WorkflowModel 1.0
import QtQuick.Dialogs 1.1
import ".." as Modeller
import "forms" as Forms
import "../../../matrix.js" as Matrix
import "../../../Global.js" as Global

Modeller.ModellerWorkArea {
   property WorkflowModel workflow;
   property OperationCatalogModel operationCatalog;
   property bool canvasActive : true;
   property var deleteItemIndex;
   property var deleteEdgeIndex;
   property int highestZIndex : 1;
    color: "transparent"
    id: canvas

    DropArea {
        id: canvasDropArea
        anchors.fill: canvas
        enabled: true
        onDropped: {
            if (drag.source.type === "singleoperation" || drag.source.type === "workflow") {
                if (drag.source.type === "workflow") {
                    operations.refresh()
                }
                var oper = wfCanvas.getOperation(drag.source.ilwisobjectid)
                wfCanvas.createItem(drag.x, drag.y,oper)

                var paramterIndexes = workflow.addOperation(drag.source.ilwisobjectid)

                var item = wfCanvas.currentItem
                wfCanvas.isInsideCondition(item.x + (item.width / 2), item.y + (item.height / 2), item.containerIndex)
                if (wfCanvas.currentConditionContainer != -1) {
                    wfCanvas.conditionBoxList[wfCanvas.currentConditionContainer].setCanvasColor(Global.mainbackgroundcolor)
                    wfCanvas.addCurrentOperationToCondition(item)
                }

                wfCanvas.canvasValid = false

                generateForm(paramterIndexes)
            }
        }
    }

    MouseArea {
        id: area
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        property bool positionChanged: false

        onWheel: {
            handleScroll(wheel);
        }

        onPressed: {
            if (canvasActive) {

                wfCanvas.canvasValid = false;

                var operationSelected = -1, highestZ = -1, smallestDistance = 100000,
                        selectedFlow = false, implicitIndexes, constantValues;

                for(var i=0; i < wfCanvas.operationsList.length; ++i){

                    var item = wfCanvas.operationsList[i]
                    var startCoords = item.getXYcoordsCanvas();

                    var endX = (startCoords.x + (item.width * item.scale));
                    var endY = (startCoords.y + (item.height * item.scale));

                    var EndCoords = transformedPoint(endX, endY);

                    var isContained = mouseX >= (startCoords.x) && mouseY >= (startCoords.y) && mouseX <= endX && mouseY <= endY;

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
                        var distanceLine = distanceAC + distanceBC;


                        // Check if mouse intersects the line with offset of 10
                        if(distanceLine >= distanceAB &&
                           distanceLine < (distanceAB + wfCanvas.scale) &&
                           distanceLine - distanceAB < smallestDistance)
                        {
                            smallestDistance = distanceLine - distanceAB;
                            selectedFlow = flow;
                        }
                        flow.isSelected = false;
                    }

                    if ( isContained && item.z > highestZ ) {
                        operationSelected = i
                        highestZ = item.z
                    }
                    item.isSelected = false
                }
                wfCanvas.oldx = mouseX
                wfCanvas.oldy = mouseY
                wfCanvas.currentIndex = operationSelected

                if(!selectedFlow && operationSelected == -1)
                {
                    wfCanvas.lastX = mouseX;
                    wfCanvas.lastY = mouseY;
                    wfCanvas.lastOpX = mouseX;
                    wfCanvas.lastOpY = mouseY;
                    wfCanvas.dragStart = transformedPoint(wfCanvas.lastX,wfCanvas.lastY);
                    wfCanvas.dragged = false;
                }

                if (selectedFlow && operationSelected == -1) {
                    selectedFlow.isSelected = true
                } else if (operationSelected > -1) {
                    item = wfCanvas.operationsList[operationSelected]
                    item.isSelected = true
                    wfCanvas.currentItem = item

                    implicitIndexes = workflow.implicitIndexes(operationSelected)
                    constantValues = workflow.getAsignedValuesByItemID(operationSelected)

                    if(implicitIndexes){
                        manager.showOperationFormWithHiddenFields(item, operationSelected, constantValues, implicitIndexes)
                    }else{
                        manager.showOperationForm(item, operationSelected, constantValues)
                    }

                    manager.showMetaData(item.operation)
                } else {
                    manager.resetMetaData();
                }
            }

        }

        onDoubleClicked: {
            var operationSelected = -1, item = 0, highestZ = 0;
            for(var i=0; i < wfCanvas.operationsList.length; ++i){

                item = wfCanvas.operationsList[i]
                var isContained = mouseX >= item.x && mouseY >= item.y && mouseX <= (item.x + item.width) && mouseY <= (item.y + item.height)

                if ( isContained && item.z > highestZ ) {
                    operationSelected = i
                    highestZ = item.z
                }
            }
            if (operationSelected > -1) {
                var resource = mastercatalog.id2Resource(item.operation.id)
                var filter = "itemid=" + resource.id
                bigthing.newCatalog(filter, "workflow",resource.url,"other")
            }
        }

        onPositionChanged: {
            wfCanvas.lastX = mouseX
            wfCanvas.lastY = mouseY
            wfCanvas.dragged = true
            if (wfCanvas.dragStart) {
                cursorShape = Qt.ClosedHandCursor

                var pt = transformedPoint(wfCanvas.lastX, wfCanvas.lastY)
                translate(pt.x - wfCanvas.dragStart.x,
                          pt.y - wfCanvas.dragStart.y)
                panOperation((wfCanvas.lastX - wfCanvas.lastOpX),
                             (wfCanvas.lastY - wfCanvas.lastOpY))
                wfCanvas.lastOpX = mouseX
                wfCanvas.lastOpY = mouseY
            }

            if (attachementForm.state == "invisible") {
                if (wfCanvas.workingLineBegin.x !== -1) {
                    wfCanvas.workingLineEnd = Qt.point(mouseX, mouseY)
                    wfCanvas.canvasValid = false
                }
                if (wfCanvas.oldx >= 0 && wfCanvas.oldy >= 0
                        && wfCanvas.currentIndex >= 0) {

                    var item = wfCanvas.operationsList[wfCanvas.currentIndex]
                    if (item) {
                        cursorShape = Qt.SizeAllCursor
                        area.positionChanged = true
                        item.x += ((mouseX - wfCanvas.oldx) / wfCanvas.scale)
                        item.y += ((mouseY - wfCanvas.oldy) / wfCanvas.scale)

                        wfCanvas.oldx = mouseX
                        wfCanvas.oldy = mouseY

                        wfCanvas.isInsideCondition((item.x + (item.width / 2)), (item.y + (item.height / 2)), item.containerIndex)

                        replacePanOperation()
                    }
                }
            }
        }

        onReleased: {
            cursorShape = Qt.ArrowCursor
            wfCanvas.stopWorkingLine()
            wfCanvas.dragStart = null;
            if(area.positionChanged) {
                var item = wfCanvas.currentItem
                var containerIndex = wfCanvas.currentConditionContainer

                //item.resetPanOperation();

                if (containerIndex !== -1) {
                    if(item.containerIndex === -1) {
                        wfCanvas.addCurrentOperationToCondition(item)
                    } else if (item.containerIndex !== containerIndex) {
                        wfCanvas.removeCurrentOperationFromCondition(item)
                        wfCanvas.addCurrentOperationToCondition(item)
                    }
                    wfCanvas.conditionBoxList[containerIndex].resize()
                    wfCanvas.conditionBoxList[containerIndex].setCanvasColor(Global.mainbackgroundcolor)

                 } else {
                    if(item.containerIndex !== -1)
                    {
                        wfCanvas.removeCurrentOperationFromCondition(item)
                    }
                    wfCanvas.parent.color = Global.mainbackgroundcolor
                }
                replacePanOperation()
                area.positionChanged = false
            }
        }
    }

   function defaultZoom(){
       var pt = transformedPoint(wfCanvas.width/2, wfCanvas.height/2)

       wfCanvas.matrix = new Matrix.Matrix()

       wfCanvas.ctx.setTransform(1, 0, 0, 1, 0, 0);
       wfCanvas.draw(true)

       for (var i = 0; i < wfCanvas.operationsList.length; i++) {
           wfCanvas.operationsList[i].scale = 1
       }

       for (var i=0; i < wfCanvas.conditionBoxList.length; i++) {
           wfCanvas.conditionBoxList[i].scale = 1
       }

       replacePanOperation()

       wfCanvas.scale = 1
       modellerDataPane.setPercentage();
   }

   function getScale()
   {
       return wfCanvas.scale;
   }

   function assignConstantInputData(inputData, itemId) {
       var parameterIndexes = workflow.assignConstantInputData(inputData, itemId)
       wfCanvas.operationsList[itemId].resetInputModel()

       generateForm(parameterIndexes)
   }

   function assignConditionInputData(result, conditionIds) {
       var parts = conditionIds.split('|')
       workflow.assignConditionInputData(result, parts)
       wfCanvas.conditionBoxList[parts[0]].refresh()
   }

   function newCondition(){
       wfCanvas.createCondition(50,50)
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
       if(flow !== 0)
       {
           messageDialogEdge.open()
       }
   }

   function alterSelectedEdge(){
       var flow = getSelectedEdge()
       if(flow !== 0)
       {
           //Retrieve target and rectangle before deleting the edge
           var target = flow.target;
           var attachedRect = flow.attachtarget;

           //Delete the edge
           var from = flow.source.itemid
           var to = flow.target.itemid
           var inputIndex = flow.flowPoints.toParameterIndex
           var outputIndex = flow.flowPoints.fromParameterIndex
           var operationObject = wfCanvas.operationsList[deleteItemIndex]

           workflow.deleteFlow(from, to, outputIndex, inputIndex)
           operationObject.flowConnections.splice(deleteEdgeIndex, 1)

           flow.target.resetInputModel()
           wfCanvas.canvasValid = false

           //Create a new edge
           wfCanvas.showAttachmentFormFromFlow(flow);
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
          var parameterIndexes = workflow.deleteOperation(deleteItemIndex)

          // Clean operation form
          manager.clearOperationForm()

          // If the operation is inside a container, remove it from the container
          if(item.containerIndex !== -1) {
              var containerIndex = item.containerIndex;
              wfCanvas.removeCurrentOperationFromCondition(item)
              wfCanvas.conditionBoxList[containerIndex].resize()

              replacePanOperation()
          }

          // This removes 1 from the operation list beginning from deleteItemIndex
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
                  wfCanvas.operationsList[i].flowConnections.splice(deleteFlows[j] - j, 1)
              }
          }
          // Loop through all operations after the deleted item. We need to reset their itemid
          for (var i = deleteItemIndex; i < wfCanvas.operationsList.length; i++) {
              wfCanvas.operationsList[i].itemid = i
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

          generateForm(parameterIndexes)
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

           var parameterIndexes = workflow.deleteFlow(from, to, outputIndex, inputIndex)
           wfCanvas.operationsList[deleteItemIndex].flowConnections.splice(deleteEdgeIndex, 1)
           flow.target.resetInputModel()
           wfCanvas.canvasValid = false

           generateForm(parameterIndexes)
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
    function generateForm(parameterIndexes) {
        if (workflow) {
            var operationNames = []

            for( var i=0; i < wfCanvas.operationsList.length; i++){
                var operationItem = wfCanvas.operationsList[i];
                operationNames[i] = {
                    inParameterCount: workflow.operationInputParameterCount(i),
                    outParameterCount: workflow.operationOutputParameterCount(i)
                };
            }
            workflow.createMetadata()
            manager.showRunForm(workflow.id, operationNames, parameterIndexes)
        }
    }

    /**
          Draws canvas from the workflow
                */
    function drawFromWorkflow() {
        var nodes = workflow.nodes, node, resource, unOrderdEdges = workflow.edges, edges = [], nodeEdges, edge, fromItemid, toItemId, fromOperation = false, toOperation = false, flowPoints
        for (var i = 0; i < unOrderdEdges.length; i++) {
            edge = unOrderdEdges[i]
            if (edge.fromVertex in edges) {
                edges[edge.fromVertex].push(edge)
            } else {
                edges[edge.fromVertex] = [edge]
            }
        }

        for (var i = 0; i < nodes.length; i++) {
            node = nodes[i]
            resource = wfCanvas.getOperation(node.operationId)

            wfCanvas.createItem(node.x/wfCanvas.scale, node.y/wfCanvas.scale, resource)
        }
        for (var i = 0; i < nodes.length; i++) {
            node = nodes[i]
            nodeEdges = edges[node.vertex]
            if (nodeEdges) {
                for (var j = 0; j < nodeEdges.length; j++) {
                    edge = nodeEdges[j]
                    fromItemid = node.vertex //TODO: Temporary
                    toItemId = edge.toVertex //TODO: Temporary

                    for (var k = 0; k < wfCanvas.operationsList.length; k++) {
                        if (!fromOperation
                                && wfCanvas.operationsList[k].itemid == fromItemid) {
                            fromOperation = wfCanvas.operationsList[k]
                        } else if (!toOperation
                                   && wfCanvas.operationsList[k].itemid == toItemId) {
                            toOperation = wfCanvas.operationsList[k]
                        }
                    }

                    if (fromOperation && toOperation) {
                        flowPoints = {
                            fromParameterIndex: edge.fromParameter,
                            toParameterIndex: edge.toParameter
                        }
                        fromOperation.flowConnections.push({
                           target: toOperation,
                           source: fromOperation,
                           attachtarget: toOperation.index2Rectangle(edge.toRect),
                           attachsource: fromOperation.index2Rectangle(edge.fromRect),
                           flowPoints: flowPoints,
                           isSelected: false
                       })
                    }

                    fromOperation = false
                    toOperation = false
                }
            }
        }
        generateForm()
    }

    Keys.onEscapePressed: {
        // TODO: doesn't work
        wfCanvas.stopWorkingLine()
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
       z: Number.MAX_VALUE

       property var ctx: getContext('2d')
       property bool canvasValid: true

       property double oldx : -1.0
       property double oldy : -1.0
       property point workingLineBegin : Qt.point(-1,-1)
       property point workingLineEnd : Qt.point(-1,-1)
       property int currentConditionContainer: -1
       property int currentIndex: 0
       property var component
       property var currentItem
       property var operationsList: []
       property var conditionBoxList: []

       property double scaleFactor: 1.1;
       property int lastX: wfCanvas.width/2;
       property int lastY: wfCanvas.height/2;
       property int lastOpX: canvas.width/2;
       property int lastOpY: canvas.height/2;
       property var dragStart;
       property bool dragged;
       property var matrix: new Matrix.Matrix();
       property double scale: 1
       property int dropSquareRadius: 50;
       property var bigDropArea: canvasDropArea

       Timer {
           interval: 30;
           running: true;
           repeat: true
           onTriggered: {
               wfCanvas.draw(true)
           }
       }

       function getConditions(containerId) {
            return canvas.workflow.getConditions(containerId)
       }

       function getOperation(id) {
           var oper = operations.operation(id);
           return oper;
       }

       function invalidate() {
           canvasValid = false;
       }

       function draw(force){
           var p1 = transformedPoint(0,0);
           var p2 = transformedPoint(width, height);
           ctx.clearRect(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);

           if (canvasValid == false || (force !== null && force)) {

               canvasValid = true
               if ( workingLineBegin.x !== -1 && workingLineEnd.x !== -1){
                   ctx.beginPath();
                   ctx.lineWidth = 3;
                   var pt1 = transformedPoint(workingLineBegin.x, workingLineBegin.y);
                   var pt2 = transformedPoint(workingLineEnd.x, workingLineEnd.y);
                   ctx.strokeStyle = "red"

                   var fromx = pt1.x
                   var fromy = pt1.y
                   var tox = pt2.x
                   var toy = pt2.y
                   var headlen = 15;   // length of head in pixels
                   var angle = Math.atan2(toy-fromy,tox-fromx);

                   ctx.moveTo(fromx, fromy);
                   ctx.lineTo(tox, toy);
                   ctx.moveTo(tox, toy);
                   ctx.lineTo(tox-headlen*Math.cos(angle-Math.PI/6),toy-headlen*Math.sin(angle-Math.PI/6));
                   ctx.moveTo(tox, toy);
                   ctx.lineTo(tox-headlen*Math.cos(angle+Math.PI/6),toy-headlen*Math.sin(angle+Math.PI/6));

                   ctx.stroke()
               }

               for( var i=0; i < operationsList.length; i++){
                   operationsList[i].drawFlows(ctx, matrix)
               }
           }

           wfCanvas.requestPaint();
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
               var ptOp = transformedPoint(x, y)

               currentItem = component.createObject(canvas, {"x": ptOp.x, "y": ptOp.y, "operation" : resource, "itemid" : operationsList.length, "scale": wfCanvas.scale});
               if (currentItem == null) {
                   // Error Handling
                   console.log("Error creating object");
               }
               operationsList.push(currentItem)
               replacePanOperation()

           } else if (component.status == Component.Error) {
               // Error Handling
               console.log("Error loading component:", component.errorString());
           }
       }

       function createCondition(x,y) {
           component = Qt.createComponent("ConditionItem.qml");
           if (component.status == Component.Ready)
               finishCreatingCondition(x,y);
           else
               component.statusChanged.connect(finishCreatingCondition);
       }

       function finishCreatingCondition(x,y) {
           if (component.status == Component.Ready) {
               var ptOp = transformedPoint(x, y)

               currentItem = component.createObject(canvas, {"x": ptOp.x, "y": ptOp.y, "scale": wfCanvas.scale, "containerId": conditionBoxList.length});
               if (currentItem == null) {
                   // Error Handling
                   console.log("Error creating object");
               }
               workflow.addConditionContainer()
               conditionBoxList.push(currentItem);
               replacePanOperation()
           } else if (component.status == Component.Error) {
               // Error Handling
               console.log("Error loading component:", component.errorString());
           }
       }

       function isInsideCondition(centreX, centreY, containerIndex) {

           for (var i = 0; i < wfCanvas.conditionBoxList.length; i++) {
               var box = wfCanvas.conditionBoxList[i]
               if (    centreX + dropSquareRadius > box.x &&
                       centreY + dropSquareRadius/2 > box.y &&
                       centreX - dropSquareRadius < (box.x + box.width) &&
                       centreY - dropSquareRadius/2 < (box.y + box.height)) {
                   if(containerIndex !== i ) {
                       box.setCanvasColor(Global.edgecolor)
                   }
                   wfCanvas.parent.color = Global.mainbackgroundcolor
                   currentConditionContainer = i
                   return
               } else {
                   box.setCanvasColor(Global.mainbackgroundcolor)
               }
           }
           if (containerIndex !== -1) {
               wfCanvas.parent.color = Global.edgecolor

           }
           currentConditionContainer = -1
       }

       function addCurrentOperationToCondition(item) {
           var box = conditionBoxList[currentConditionContainer]
           item.containerIndex = currentConditionContainer
           box.addToOperationList(item.itemid)
           workflow.addOperationToContainer(currentConditionContainer, item.itemid)
           currentConditionContainer = -1
       }

       function removeCurrentOperationFromCondition(item) {
           var box = conditionBoxList[item.containerIndex]
           box.removeFromOperationList(item.itemid)
           workflow.removeOperationFromContainer(item.containerIndex, item.itemid)
           item.containerIndex = -1
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
            for (var i = 0; i < wfCanvas.operationsList.length; i++) {
                wfCanvas.operationsList[i].deselectAll()
            }
       }

       function showConditionTypeForm(id){
           canvasActive = false;
           conditionTypeForm.state = "visible"
           conditionTypeForm.containerId = id
       }

       function showAttachmentForm(target, attachRect){
           canvasActive = false;
           attachementForm.operationOut = operationsList[wfCanvas.currentIndex]
           attachementForm.operationIn = target
           attachementForm.attachRect = attachRect
           attachementForm.target = target
           attachementForm.state = "visible"
       }

       function showAttachmentFormFromFlow(flow) {
           canvasActive = false;
           attachementForm.operationOut = flow.source
           attachementForm.operationIn = flow.target
           attachementForm.attachRect = flow.attachtarget
           attachementForm.target = flow.target
           attachementForm.source = flow.source
           attachementForm.state = "visible"
       }

       onWidthChanged: {
           // force re-draw if the ModellerPanel width has changed
           invalidate();
       }

       onHeightChanged: {
           // force re-draw if the ModellerPanel height has changed
           wfCanvas.draw(true);
       }

       Forms.FlowParametersChoiceForm{
           id : attachementForm
       }
       Forms.ConditionTypeChoiceForm{
           id : conditionTypeForm
       }
   }

    Component.onDestruction: {
        var coordinates = [], node
        for (var i = 0; i < wfCanvas.operationsList.length; i++) {
            node = wfCanvas.operationsList[i]
            coordinates[node.itemid] = node.x + '|' + node.y
        }

        workflow.store(coordinates)
    }

    function zoom(clicks) {
        if (wfCanvas.workingLineBegin.x === -1 && wfCanvas.workingLineEnd.x === -1) {
            var pt;
            if(area.containsMouse){
                pt = transformedPoint(wfCanvas.lastX, wfCanvas.lastY)
            }else{
                pt = transformedPoint(wfCanvas.width/2, wfCanvas.height/2)
            }

            translate(pt.x, pt.y)

            var factor = Math.pow(wfCanvas.scaleFactor, clicks)
            wfCanvas.scale *= factor

            scale(factor, factor)
            scaleOperation(factor)

            translate(-pt.x, -pt.y)

            replacePanOperation()

            modellerDataPane.setPercentage();
        }
    }

    function handleScroll(wheel) {

        var delta = wheel.angleDelta.y / 40
        if (delta)
            zoom(delta)
    }

    function scale(sx, sy) {
        wfCanvas.matrix = wfCanvas.matrix.scaleX(sx)
        wfCanvas.matrix = wfCanvas.matrix.scaleY(sy)

        wfCanvas.ctx.scale(sx, sy)
    }

    function translate(dx, dy) {
        wfCanvas.matrix = wfCanvas.matrix.translate(dx, dy)
        wfCanvas.ctx.translate(dx, dy)
    }

    function panOperation(x, y) {
        for (var i = 0; i < wfCanvas.operationsList.length; i++) {
            wfCanvas.operationsList[i].panOperation(x, y)
        }

        for (var i=0; i < wfCanvas.conditionBoxList.length; i++) {
            wfCanvas.conditionBoxList[i].panOperation(x, y);
        }
    }

    function replacePanOperation() {
        for (var i = 0; i < wfCanvas.operationsList.length; i++) {
            //var xy = wfCanvas.operationsList[i].getXYcoords();
            var ptOp = getScreenCoords(wfCanvas.operationsList[i].x,
                                       wfCanvas.operationsList[i].y)
            wfCanvas.operationsList[i].replacePanOperation(ptOp.x, ptOp.y)
        }

        for (var i=0; i < wfCanvas.conditionBoxList.length; i++) {
            //var xy = wfCanvas.operationsList[i].getXYcoords();
            var ptOp = getScreenCoords(wfCanvas.conditionBoxList[i].x,
                                       wfCanvas.conditionBoxList[i].y)
            wfCanvas.conditionBoxList[i].replacePanOperation(ptOp.x, ptOp.y)
        }
    }

    function scaleOperation(scaleFactor) {
        for (var i = 0; i < wfCanvas.operationsList.length; i++) {
            wfCanvas.operationsList[i].scaleOperation(scaleFactor)
        }

        for (var i=0; i < wfCanvas.conditionBoxList.length; i++) {
            wfCanvas.conditionBoxList[i].scaleOperation(scaleFactor)
        }
    }

    function transformedPoint(x, y) {
        return {
            x: wfCanvas.matrix.inverse().a * x + wfCanvas.matrix.inverse().c * y + wfCanvas.matrix.inverse().e,
            y: wfCanvas.matrix.inverse().b * x + wfCanvas.matrix.inverse().d * y + wfCanvas.matrix.inverse().f
        }
    }

    function getScreenCoords(x, y) {
        var xn = wfCanvas.matrix.e + x * wfCanvas.matrix.a
        var yn = wfCanvas.matrix.f + y * wfCanvas.matrix.d
        return {
            x: xn,
            y: yn
        }
    }
}
