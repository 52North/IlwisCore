import QtQuick 2.0;
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import WorkflowModel 1.0

ModellerWorkArea {
    property WorkflowModel workflow;

    Canvas {

        id : wfCanvas
        anchors.fill: parent

        property string drawColor: "black"
        property var ctx: getContext('2d')
        // indicator whether the Canvas sould be re-drawn
        property bool canvasValid: true

        // indiocator whether the object should be dragged
        property bool isDrag: false
        property BasicWorkflowDrawObject onPressedSelectedElement
        property BasicWorkflowDrawObject currentSelectedElement
        property double oldx : -1.0
        property double oldy : -1.0
        property point workingLineBegin : Qt.point(-1,-1)
        property point workingLineEnd : Qt.point(-1,-1)
        property int currentIndex : 0

        /*
     * Timer for periodical redrawing of Canvas
     */
        Timer {
            interval: 30;
            running: true;
            repeat: true
            onTriggered: wfCanvas.draw()
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

        /*
     * Draw function, is called by timer
     */
        function draw(force){
            if (canvasValid == false || (force !== null && force)) {
                clear(ctx);
                canvasValid = true
                if ( workingLineBegin.x !== -1 && workingLineEnd.x !== -1){
                    ctx.beginPath();
                    ctx.lineWidth = 2;
                    ctx.moveTo(workingLineBegin.x, workingLineBegin.y);
                    ctx.strokeStyle = "red"
                    ctx.lineTo(workingLineEnd.x, workingLineEnd.y);
                    ctx.stroke();
                }
                for( var i=0; i < operationsList.length; ++i){
                    operationsList[i].drawFlows(ctx)
                }
                //wfCanvas.requestPaint();
            }
        }

        property var component;
        property var currentItem;
        property var operationsList: []
        property int count : 0

        function createItem(x,y, resource) {
            component = Qt.createComponent("OperationItem.qml");
            if (component.status == Component.Ready)
                finishCreation(x,y,resource);
            else
                component.statusChanged.connect(finishCreation);
        }

        function finishCreation(x,y,resource) {
            if (component.status == Component.Ready) {
                currentItem = component.createObject(wfCanvas, {"x": x, "y": y, "operation" : resource,"itemid" : count});
                if (currentItem == null) {
                    // Error Handling
                    console.log("Error creating object");
                }
                operationsList.push(currentItem)
                workflow.addOperation(count, resource.id)
                ++count

            } else if (component.status == Component.Error) {
                // Error Handling
                console.log("Error loading component:", component.errorString());
            }
        }

        /*
     * Clear the Canvas
     */
        function clear() {
            if ( ctx){
                ctx.reset();
                ctx.clearRect(0, 0, width, height);
                ctx.stroke();
                wfCanvas.requestPaint();
            }
        }

        function stopWorkingLine(){
            wfCanvas.oldx = -1.0
            wfCanvas.oldy = -1.0
            wfCanvas.onPressedSelectedElement = null;
            wfCanvas.workingLineBegin = Qt.point(-1,-1)
            wfCanvas.workingLineEnd = Qt.point(-1,-1)
            wfCanvas.canvasValid = true
        }

        function showAttachementForm(yesno, target, attachRect){
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
                if (drag.source.type === "singleoperation") {
                    var oper = wfCanvas.getOperation(drag.source.ilwisobjectid)
                    wfCanvas.createItem(drag.x - 50, drag.y - 30,oper)
                    workflow.addOperation(wfCanvas.count, drag.source.ilwisobjectid)

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
                for(var i=0; i < wfCanvas.operationsList.length; ++i){
                    var item = wfCanvas.operationsList[i]
                    var isContained = mouseX >= item.x && mouseY >= item.y && mouseX <= (item.x + item.width) && mouseY <= (item.y + item.height)
                    if ( isContained) {
                        wfCanvas.oldx = mouseX
                        wfCanvas.oldy = mouseY
                        wfCanvas.currentIndex = i;
                        item.isSelected = true
                        manager.showOperationForm(item.operation.id)
                    }else
                        item.isSelected = false
                }
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
