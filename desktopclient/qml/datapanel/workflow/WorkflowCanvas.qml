import QtQuick 2.0;
import MasterCatalogModel 1.0

ModellerWorkArea {
Canvas {

    id : wfCanvas
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
                wfCanvas.requestPaint();
            }
            if ( workingLineBegin.x !== -1 && workingLineEnd.x !== -1){
                     ctx.beginPath();
                     ctx.lineWidth = 2;
                     ctx.moveTo(workingLineBegin.x, workingLineBegin.y);
                     ctx.strokeStyle = "red"
                     ctx.lineTo(workingLineEnd.x, workingLineEnd.y);
                     ctx.stroke();
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
            currentItem = component.createObject(wfCanvas, {"x": x, "y": y, "name" : name});
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
            wfCanvas.requestPaint();
        }
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
                var oper = wfCanvas.getResource(drag.source.ilwisobjectid)
                wfCanvas.createItem(drag.x - 50, drag.y - 30,oper.displayName)
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
        hoverEnabled: wfCanvas.workingLineBegin.x !== -1

        onPressed: {
            for(var i=0; i < wfCanvas.operations.length; ++i){
                 var item = wfCanvas.operations[i]
                var isContained = mouseX >= item.x && mouseY >= item.y && mouseX <= (item.x + item.width) && mouseY <= (item.y + item.height)
                if ( isContained) {
                    wfCanvas.oldx = mouseX
                    wfCanvas.oldy = mouseY
                    wfCanvas.currentIndex = i;
                    item.isSelected = true
                }else
                    item.isSelected = false
            }
        }

        onPositionChanged: {
            if ( wfCanvas.workingLineBegin.x !== -1){
                wfCanvas.workingLineEnd = Qt.point( mouseX, mouseY)
                wfCanvas.canvasValid = false
            }
            if ( wfCanvas.oldx >= 0 && wfCanvas.oldy >= 0 && wfCanvas.currentIndex >= 0)    {

                var item = wfCanvas.operations[wfCanvas.currentIndex]
                if ( item){
                    item.x += ( mouseX - wfCanvas.oldx)
                    item.y += (mouseY - wfCanvas.oldy)
                    wfCanvas.oldx = mouseX
                    wfCanvas.oldy = mouseY
                }
            }
        }

        onReleased: {
            wfCanvas.oldx = -1.0
            wfCanvas.oldy = -1.0
            wfCanvas.currentIndex = -1
            wfCanvas.onPressedSelectedElement = null;
            wfCanvas.workingLineBegin = Qt.point(-1,-1)
            wfCanvas.workingLineEnd = Qt.point(-1,-1)
            wfCanvas.canvasValid = true
        }
    }


}
}
