import QtQuick 2.0;
import MasterCatalogModel 1.0

ModellerWorkArea {
Canvas {

    id : workflowDrawingCanvas
    anchors.fill: parent

    //anchors.fill: parent
    // context related values
    property int lineWidth: 2
    property string drawColor: "black"
    property var ctx: getContext('2d')
    // indicator whether the Canvas sould be re-drawn
    property bool canvasValid: true
    // elements to be drawn
    property variant elements: []
    // current workflow
    property string workflow;

    // indiocator whether the object should be dragged
    property bool isDrag: false
    // indicator whether PressAndHold is active
    property bool isPressAndHold: false
    property BasicWorkflowDrawObject onPressedSelectedElement
    property BasicWorkflowDrawObject currentSelectedElement
    property double oldx : -1.0
    property double oldy : -1.0
    property int currentIndex : 0

    /*
     * Timer for periodical redrawing of Canvas
     */
    Timer {
        interval: 30;
        running: true;
        repeat: true
        onTriggered: workflowDrawingCanvas.draw()
    }


    /*
     * Get the resource by id from MasterCatalog
     */
    function getResource(id) {
        return mastercatalog.id2Resource(id);
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
            if ( elements){
                var l = elements.length
                for (var i = 0; i < l; i++) {
                    elements[i].draw(ctx);
                }
                workflowDrawingCanvas.requestPaint();
            }
        }
    }

    property var component;
    property var currentItem;
    property var operations: []

    function createItem(x,y, name) {
        component = Qt.createComponent("OperationItem.qml");
        if (component.status == Component.Ready)
            finishCreation(x,y,name);
        else
            component.statusChanged.connect(finishCreation);
    }

    function finishCreation(x,y,name) {
        if (component.status == Component.Ready) {
            currentItem = component.createObject(workflowDrawingCanvas, {"x": x, "y": y, "name" : name});
            if (currentItem == null) {
                // Error Handling
                console.log("Error creating object");
            }
            operations.push(currentItem)

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
            workflowDrawingCanvas.requestPaint();
        }
    }

    onWidthChanged: {
        // force re-draw if the ModellerPanel width has changed
        invalidate();
    }

    onHeightChanged: {
        // force re-draw if the ModellerPanel height has changed
        workflowDrawingCanvas.draw(true);
    }

    DropArea {
        id: canvasDropArea
        anchors.fill: workflowDrawingCanvas
        onDropped: {
            if (drag.source.type === "singleoperation") {
                var oper = workflowDrawingCanvas.getResource(drag.source.ilwisobjectid)
                workflowDrawingCanvas.createItem(drag.x - 50, drag.y - 30,oper.displayName)
            }
           // var workflowModel = getResource(workflow);

        }
    }

    onWorkflowChanged: {
        console.log("Worklfow changed")
        var workflowModel = getResource(workflow);
        /*
         * TODO get workflow (Datasource -> Operations -> ... )  from WorkflowModel, if exist and create drawable elements
         * Store states in the WorkflowModel object
         *
         */
        invalidate();
    }


    MouseArea {
        id: area
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPressed: {
            for(var i=0; i < workflowDrawingCanvas.operations.length; ++i){
                 var item = workflowDrawingCanvas.operations[i]
                var isContained = mouseX >= item.x && mouseY >= item.y && mouseX <= (item.x + item.width) && mouseY <= (item.y + item.height)
                if ( isContained) {
                    workflowDrawingCanvas.oldx = mouseX
                    workflowDrawingCanvas.oldy = mouseY
                    workflowDrawingCanvas.currentIndex = i;
                    break
                }
            }
        }

        onPositionChanged: {
            if ( workflowDrawingCanvas.isPressAndHold){
                if ( workflowDrawingCanvas.oldx >= 0 && workflowDrawingCanvas.oldy >= 0 && workflowDrawingCanvas.currentIndex >= 0)    {
                    var item = workflowDrawingCanvas.operations[workflowDrawingCanvas.currentIndex]
                    if ( item){
                        item.x += ( mouseX - workflowDrawingCanvas.oldx)
                        item.y += (mouseY - workflowDrawingCanvas.oldy)
                        workflowDrawingCanvas.oldx = mouseX
                        workflowDrawingCanvas.oldy = mouseY
                    }
                }
            }
        }

        onPressAndHold: {
            workflowDrawingCanvas.isPressAndHold = true;
        }

        onReleased: {
            workflowDrawingCanvas.oldx = -1.0
            workflowDrawingCanvas.oldy = -1.0
            workflowDrawingCanvas.currentIndex = -1
            if (workflowDrawingCanvas.isPressAndHold) {
                //clearSelections();
            }
            workflowDrawingCanvas.onPressedSelectedElement = null;
            if (mouse.button == Qt.LeftButton) {

            }
        }
    }


}
}
