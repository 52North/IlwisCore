import QtQuick 2.1
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import WorkflowModel 1.0
import QtQuick.Dialogs 1.1
import ".." as Modeller
import "forms" as Forms
import "../../../matrix.js" as Matrix
import "../../../Global.js" as Global


Canvas {
    id : wfcanvas

    property var component
    property var ctx: getContext('2d')
    property point workingLineBegin : Qt.point(-1,-1)
    property point workingLineEnd : Qt.point(-1,-1)
    property double oldx : -1.0
    property double oldy : -1.0
    property bool canvasValid: true
    property double zoomScale : 1.0
    property double oldZoomScale : 1.0
    clip : true

    onWidthChanged: {
        canvasValid = false
    }

    onHeightChanged: {
        canvasValid = false
    }

    FlowParametersChoiceForm{
        id : attachementForm
    }

    TestDetailFlowChoiceForm{
        id : testForm
    }



    WFCanvasMouseArea{
        id :mousearea
    }


    function createItem(x,y, resource, nodeId) {
        component = Qt.createComponent("OperationItem.qml");
        if (component.status == Component.Ready)
            finishCreation(x,y,resource, nodeId);
        else
            component.statusChanged.connect(finishCreation);

    }

    function finishCreation(x,y,resource, nodeId) {
        if (component.status === Component.Ready) {

            currentItem = component.createObject(wfcanvas, {"x": x, "y":y, "operation" : resource, "itemid" : nodeId, "scale": wfCanvas.scale});
            if (currentItem == null) {
                // Error Handling
                console.log("Error creating object");
            }
            operationsList.push(currentItem)
        }
    }

    function draw(force){
        ctx.clearRect(0,0,width, height);

        if (canvasValid == false || (force !== null && force)) {

            canvasValid = true
            if ( workingLineBegin.x !== -1 && workingLineEnd.x !== -1){
                ctx.beginPath();
                ctx.lineWidth = 3;
                var pt1 = Qt.point(workingLineBegin.x, workingLineBegin.y)
                var pt2 = Qt.point(workingLineEnd.x, workingLineEnd.y)
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
                operationsList[i].drawFlows(ctx)
            }

            for( i=0; i < conditionsList.length; i++){
                conditionsList[i].drawFlows(ctx)
            }

        }

        wfCanvas.requestPaint();
    }

    function showAttachmentForm(target, attachRect){
        canvasActive = false;
        attachementForm.operationOut = workarea.currentItem
        attachementForm.operationIn = target
        attachementForm.attachRect = attachRect
        attachementForm.target = target
        attachementForm.state = "visible"
    }

    function showTestForm(target, attachRect, type, toIndex){
        canvasActive = false;
        testForm.operationFrom = workarea.currentItem
        testForm.conditionTo = target
        testForm.attachRect = attachRect
        testForm.toType = type
        testForm.toIndex = toIndex
        testForm.state = "visible"
    }

    function stopWorkingLine(){
        oldx = -1.0
        oldy = -1.0
        workingLineBegin = Qt.point(-1,-1)
        workingLineEnd = Qt.point(-1,-1)
        canvasValid = true
        for (var i = 0; i < operationsList.length; i++) {
            operationsList[i].deselectAll()
        }
    }


}
