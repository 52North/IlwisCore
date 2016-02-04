import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import OperationModel 1.0
import WorkflowErrorModel 1.0
import ErrorModel 1.0

Rectangle {
    id : operationItem
    property int containerIndex : -1
    width: 200
    height: 120
    transformOrigin: Item.TopLeft;
    z: 0
    color:"transparent"
    transform: Translate { id: transformTl }

    function isSelected(matrix, mouseX, mouseY)
    {
        var startX = (operationItem.x + transformTl.x);
        var startY = (operationItem.y + transformTl.y);
        var endX = (operationItem.x + transformTl.x + operationItem.width);
        var endY = (operationItem.y + transformTl.y + operationItem.height);

        var ptEnd = getScreenCoords(matrix, endX, endY);


        var isContained = mouseX >= (operationItem.x + transformTl.x) && mouseY >= (operationItem.y + transformTl.y) && mouseX <= (operationItem.x + transformTl.x + operationItem.width) && mouseY <= (operationItem.y + transformTl.y + operationItem.height)
        return isContained;
    }

    function panOperation(x, y)
    {
        transformTl.x += x;
        transformTl.y += y;
    }

    function getXYcoordsCanvas()
    {
        var pt = {x: 0, y: 0};
        pt.x = operationItem.x + transformTl.x;
        pt.y = operationItem.y + transformTl.y;
        return pt;
    }

    function replacePanOperation(x, y)
    {
        transformTl.x = x - operationItem.x ;
        transformTl.y = y - operationItem.y ;
    }

    function resetPanOperation()
    {
        transformTl.x = 0;
        transformTl.y = 0;
    }

    function scaleOperation(scaleFactor)
    {
        operationItem.scale *= scaleFactor;
    }

    property OperationModel operation
    property int itemid
    property var selectedAttach
    property bool isSelected : false
    property var flowConnections: []

    property alias name:operationName

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../../../images/" + name
        return iconP
    }


    function resetInputModel(){
        operationInParameters.model = null
        operationInParameters.model = operation.inParamNames
    }

    function getBackground() {
        if (operation) {
            var keywords = operation.keywords.split(', ')
            if (keywords.indexOf('workflow') > -1) {
                return iconsource("workflowitem.png")
            }
        }
        return iconsource("operationitem.png")
    }

    function getTitle() {
        return operationName.text
    }

    Image {
        id : box
        anchors.fill: parent
        source : getBackground()
    }
    Text{
        id : operationName
        anchors.top : box.top
        anchors.topMargin: 4
        width : box.width -10
        elide: Text.ElideMiddle

        x : 15
        text : itemid + ". " + (operation ? operation.name : "?")
        font.bold : true
    }
    Text {
        id : labelInput
        anchors.top : operationName.bottom
        anchors.topMargin: 4
        font.pixelSize: 10
        x : 15
        text : qsTr("Input parameters")
        font.bold: true
    }

    ListView{
        id : operationInParameters
        anchors.top : labelInput.bottom
        anchors.topMargin: 3
        height : 30
        width : box.width
        clip : true
        x : 15
        model : operation ? operation.inParamNames : null
        interactive: false
        delegate:
            Item {
            width : box.width
            height: 10
            Row {
                spacing : 4
                width : box.width - 15
                height : 10
                Text{
                    text : index
                    width : 20
                    height : 10
                    font.pixelSize: 9
                }

                Text {
                    text : modelData
                    font.bold: !workflow.hasValueDefined(itemid, index) ? true : false
                    font.strikeout: workflow.hasValueDefined(itemid, index) ? true : false
                    width : parent.width - 30
                    height : 10
                    font.pixelSize: 9
                    elide: Text.ElideMiddle
                }
            }
        }
    }

    Text {
        id : labelOutput
        anchors.top : operationInParameters.bottom
        anchors.topMargin: 3
        font.pixelSize: 10
        x : 15
        text : qsTr("Output parameters")
        font.bold: true
    }

    ListView{
        id : operationOutParameters
        anchors.top : labelOutput.bottom
        anchors.bottom: operationItem.bottom
        anchors.bottomMargin: 2
        width : box.width
        clip : true
        interactive: false
        x : 15
        model : operation ? operation.outParamNames : null
        delegate:
            Item {
            width : box.width
            height: 10
            Row {
                spacing : 4
                width : box.width - 15
                height : 10
                Text{
                    text : index
                    width : 20
                    height : 10
                    font.pixelSize: 9
                }

                Text {
                    text : modelData
                    height : 10
                    font.pixelSize: 9
                    elide: Text.ElideMiddle
                    width : parent.width - 22
                }
            }
        }

    }


    onXChanged: {
        wfCanvas.canvasValid = false
    }
    onYChanged: {
        wfCanvas.canvasValid = false
    }

    onIsSelectedChanged: {
        if (isSelected) {
            z = highestZIndex++
        }
    }

    function deselectAll(){
        att1.isSelected = att2.isSelected = att3.isSelected = att4.isSelected = att5.isSelected = att6.isSelected = att7.isSelected = att8.isSelected = false
    }

    function transformedPoint(matrix, x, y)
    {
        return {
            x: matrix.inverse().a * x + matrix.inverse().c * y + matrix.inverse().e,
            y: matrix.inverse().b * x + matrix.inverse().d * y + matrix.inverse().f
        }
    }

    function getScreenCoords(matrix, x, y) {
        var xn = matrix.e + x * matrix.a;
        var yn = matrix.f + y * matrix.d;
        return { x: xn, y: yn };
    }

    function drawFlows(ctx, matrix){
        if (matrix){
            for(var i=0; i < flowConnections.length; i++){
                var item = flowConnections[i]

                var startPoint = item.attachsource.center()
                var endPoint = item.attachtarget.center()

                var pt1 = transformedPoint(matrix, startPoint.x, startPoint.y);
                var pt2 = transformedPoint(matrix, endPoint.x, endPoint.y);

                var fromx = pt1.x
                var fromy = pt1.y
                var tox = pt2.x
                var toy = pt2.y
                var headlen = 15;   // length of head in pixels
                var angle = Math.atan2(toy-fromy,tox-fromx);

                ctx.beginPath();
                ctx.moveTo(fromx, fromy);
                ctx.lineTo(tox, toy);
                ctx.moveTo(tox, toy);
                ctx.lineTo(tox-headlen*Math.cos(angle-Math.PI/6),toy-headlen*Math.sin(angle-Math.PI/6));
                ctx.moveTo(tox, toy);
                ctx.lineTo(tox-headlen*Math.cos(angle+Math.PI/6),toy-headlen*Math.sin(angle+Math.PI/6));

                if(item.isSelected)
                {
                    ctx.strokeStyle = "red"
                    ctx.lineWidth = 5
                }else {
                    ctx.strokeStyle = "blue"
                    ctx.lineWidth = 3
                }

                ctx.stroke()

                if ( item.flowPoints){
                    var p1 = item.flowPoints.fromParameterIndex
                    var p2 = item.flowPoints.toParameterIndex
                    var xcenter = (fromx + tox) / 2
                    var ycenter = (fromy + toy) / 2
                    var label = p1 + " -> "+  p2
                    ctx.fillStyle="#D8F6CE";
                    ctx.fillRect(xcenter - 15 ,ycenter - 10,35,15);
                    ctx.fillStyle = "#000";
                    ctx.fillText(label, xcenter-10, ycenter + 2);
                }
            }
        }
    }

    function setFlow(target, attachRect, flowPoints){
        flowConnections.push({
            "target" : target,
            "source" :operationItem,
            "attachtarget" : attachRect,
            "attachsource" : selectedAttach,
            "flowPoints" : flowPoints,
            "isSelected" : false
        })
        var parameterIndexes = workflow.addFlow(
            itemid,
            target.itemid,
            flowPoints,
            selectedAttach.index,
            attachRect.index
        )
        target.resetInputModel()
        wfCanvas.stopWorkingLine()

        canvas.generateForm(parameterIndexes)
    }

    function attachFlow(target, attachRect){
        //If not connected to itself
        if ( wfCanvas.operationsList[wfCanvas.currentIndex] !== target){

            if (operation.needChoice(target.operation)) {
                wfCanvas.showAttachmentForm(target, attachRect)
            } else {
                wfCanvas.operationsList[wfCanvas.currentIndex].setFlow(target, attachRect, null)
            }

            wfCanvas.canvasValid = false
        }
    }

    function index2Rectangle(index) {
        for (var i = 0; i < operationItem.children.length; i++) {
            var child = operationItem.children[i];
            if(child.hasOwnProperty("index") && child.index == index){
                return child
            }
        }
        return 0
    }

    AttachmentRectangle{
        id : att1
        index : 0
        anchors.top: operationItem.top
        anchors.topMargin: -3
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/4 - 5
    }
    AttachmentRectangle{
        id : att2
        index : 1
        anchors.top: operationItem.top
        anchors.topMargin: -3
        anchors.left : operationItem.left
        anchors.leftMargin: 3 * operationItem.width/4
    }
    AttachmentRectangle{
        id : att3
        index : 2
        anchors.top: operationItem.top
        anchors.topMargin: -3
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/2 - 5
    }

    AttachmentRectangle{
        id : att4
        index : 3
        anchors.bottom: operationItem.bottom
        anchors.bottomMargin: -3
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/4 - 5
    }
    AttachmentRectangle{
        id : att5
        index : 4
        anchors.bottom: operationItem.bottom
        anchors.bottomMargin: -3
        anchors.left : operationItem.left
        anchors.leftMargin: 3 * operationItem.width/4
    }

    AttachmentRectangle{
        id : att6
        index : 5
        anchors.bottom: operationItem.bottom
        anchors.bottomMargin: -3
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/2 - 5
    }

    AttachmentRectangle{
        id : att7
        index : 6
        anchors.left: operationItem.left
        anchors.leftMargin: -3
        anchors.top : operationItem.top
        anchors.topMargin: operationItem.height/2
    }

    AttachmentRectangle{
        id : att8
        index : 7
        anchors.right: operationItem.right
        anchors.rightMargin: -3
        anchors.top : operationItem.top
        anchors.topMargin: operationItem.height/2
    }
}

