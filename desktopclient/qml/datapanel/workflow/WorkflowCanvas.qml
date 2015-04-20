import QtQuick 2.0;
import MasterCatalogModel 1.0

Canvas {

    id : workflowDrawingCanvas

    anchors.fill: parent
    // context related values
    property int lineWidth: 2
    property string drawColor: "black"
    property var ctx: getContext('2d')
    property bool canvasValid: true
    // elements
    property variant elements: []
    property var frame
    property string workflow;

    property bool isDrag: false
    property bool isPressAndHold: false
    property BasicWorkflowDrawObject onPressedSelectedElement
    property BasicWorkflowDrawObject currentSelectedElement

    Timer {
        interval: 30;
        running: true;
        repeat: true
        onTriggered: workflowDrawingCanvas.draw()
    }

    function addElement(elem) {
        var count = containsOperationWorkflowDrawObject();
        var y = 300*count + 300;
        elem.setCoordinates(workflowDrawingCanvas.width/2, y)
        var t = elements;
        t.push(elem);
        elements = t;
    }

    function containsOperationWorkflowDrawObject() {
        var count = 0;
        var l = elements.length
        for (var i = 0; i < l; i++) {
            if (elements[i].nameText === "Operation") {
               count++;
            }
        }
        return count;
    }

    function getResource(id) {
        return mastercatalog.id2Resource(id);
    }

    function addOperationParameter(element){
        var operation = getResource(element.resourceId);
        // TODO
        // get operation parameter/linked sources and draw

        // currently adds dummy input/output
        var result = element.frame.currentAppForm.formresult;
        var resultArray = result.split("|");
        var l = resultArray.length
        for(var i = 0; i < l-1; i++) {
           createInput(element, resultArray[i], i, l-1);
        }
        createOutput(element);
    }

     function createInput(element, value, index, size) {
         var input = Qt.createComponent("DatasourceWorkflowDrawObject.qml").createObject();
         var inputConnector = Qt.createComponent("ConnectorWorkflowDrawObject.qml").createObject();
         addElement(input);
         addElement(inputConnector);
         input.setCoordinates(calculateX(element.x, index, size), element.y - 200);
         input.title = "Input " + (index + 1) + ":";
         input.name = value
         input.childObject = inputConnector;
         inputConnector.addParentObject(input);
         inputConnector.setStartCoordinate(input.getOutputConnector().x, input.getOutputConnector().y);

         element.addParentObject(inputConnector);
         inputConnector.childObject = element;
         inputConnector.setEndCoordinate(element.getInputConnector().x, element.getInputConnector().y);
    }

     function calculateX(x, index, size) {
         return 100 + (workflowDrawingCanvas.width/size * index);
     }

    function createOutput(element) {
        var output = Qt.createComponent("DatasourceWorkflowDrawObject.qml").createObject();
        var outputConnector = Qt.createComponent("ConnectorWorkflowDrawObject.qml").createObject();
        addElement(output);
        addElement(outputConnector);
        output.setCoordinates(element.x, element.y + 200);

        var result = element.frame.currentAppForm.formresult;
        var resultArray = result.split("|");
        var l = resultArray.length
        var resultOutput =resultArray[l-1];
        output.title = "Output:";
        output.name = resultOutput
        element.childObject = outputConnector;
        outputConnector.addParentObject(element);
        outputConnector.setStartCoordinate(element.getOutputConnector().x, element.getOutputConnector().y);

        output.addParentObject(outputConnector);
        outputConnector.childObject = output;
        outputConnector.setEndCoordinate(output.getInputConnector().x, output.getInputConnector().y);
    }


    /*
     * Set canvas validation flag to false
     */
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
            var l = elements.length
            for (var i = 0; i < l; i++) {
                elements[i].draw(ctx);
            }
            workflowDrawingCanvas.requestPaint();
        }
    }

    function clear() {
        ctx.reset();
        ctx.clearRect(0, 0, width, height);
        ctx.stroke();
        workflowDrawingCanvas.requestPaint();
    }

    function checkForElementAt(mouseX, mouseY) {
        var l = elements.length;
        var element = null;
        for (var i = l-1; i >= 0; i--) {
            elements[i].selected = false;
            if (elements[i].isSelected(mouseX, mouseY)) {
                element = elements[i];
            }
        }
        return element;
    }

    function clearSelections() {
        var l = elements.length;
        for (var i = l-1; i >= 0; i--) {
            elements[i].selected = false;
        }
        currentSelectedElement = null;
        isDrag = false;
        invalidate();
    }

    function resetCurrentSelectedElement() {
        if (currentSelectedElement !== null) {
            currentSelectedElement.selected = false;
            currentSelectedElement = null;
        }
        invalidate();
    }

    function isFirstOperation() {
        var l = elements.length
        for (var i = 0; i < l; i++) {
            if (elements[i].nameText === "Operation") {
                return false;
            }
        }
        return true;
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
//            console.log("------------------");
//            console.log(drag.source.name);
//            console.log(drag.source.ilwisobjectid);
//            console.log(drag.source.url);
//            console.log(drag.source.frame);
//            console.log(drag.source.frame.currentAppForm.formresult);
//            console.log(drag.source.type);
//            console.log("------------------");
            if (drag.source.type === "operation") {
                if (isFirstOperation()) {
                    var element = Qt.createComponent("OperationWorkflowDrawObject.qml").createObject();
                    addElement(element);
                    element.resourceId = drag.source.ilwisobjectid
                    element.name = drag.source.name
                    element.frame = drag.source.frame
                    frame = drag.source.frame
                    addOperationParameter(element);
                    invalidate();
                    frame.formResultChanged.connect(invalidate);
                } else {
                    console.log("Currently only one operation can be added to the workflow!")
                }
            }

            var workflowModel = getResource(workflow);

        }

        onEntered: {
            console.log("canvasDropArea entered by " + drag.source);
        }
    }

    onWorkflowChanged: {
        console.log("Worklfow changed")
        var workflowModel = getResource(workflow);
        // TODO get workflow (Datasource -> Operations -> ... )  from WorkflowModel, if exist and create drawable elements

        invalidate();
    }


    MouseArea {
        id: area
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                var element = checkForElementAt(mouseX, mouseY)
                if(element !== null) {
                    onPressedSelectedElement = element;
                    isDrag = true;
                }
            }
        }

        onPositionChanged: {
            if (onPressedSelectedElement != null) {
                if (isDrag) {
                    onPressedSelectedElement.setCoordinates(mouseX, mouseY);
                    invalidate();
                }
            }
        }

        onPressAndHold: {
            isPressAndHold = true;
        }

        onReleased: {
            isDrag = false;
            if (isPressAndHold) {
                clearSelections();
            }
            onPressedSelectedElement = null;
            if (mouse.button == Qt.LeftButton) {

            }
        }

        onClicked: {
            isDrag = false;
            if (mouse.button == Qt.LeftButton) {
                var element = checkForElementAt(mouseX, mouseY);
                if (element !== null) {
                    currentSelectedElement = element;
                    currentSelectedElement.selected = true;
                    invalidate();
                }
            } else if (mouse.button == Qt.RightButton) {
                clearSelections();
            }
        }
    }

}

