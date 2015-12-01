import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import OperationModel 1.0

Rectangle {
    id : operationItem
    width: 200
    height: 120
    property OperationModel operation
    property int itemid
    property var selectedAttach
    property bool isSelected : false
    property var flowConnections: []

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
        var keywords = operation.keywords.split(', ')
        if (keywords.indexOf('workflow') > -1) {
            return iconsource("workflowitem.png")
        } else {
            return iconsource("operationitem.png")
        }

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
        text : operation ? operation.name : "?"
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
                    width : parent.width - 30
                    height : 10
                    font.pixelSize: 9
                    color: workflow.hasValueDefined(itemid, index) ? "green" : "red"
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

    function deselectAll(){
        att1.isSelected = att2.isSelected = att3.isSelected = att4.isSelected = att5.isSelected = att6.isSelected = att7.isSelected = att8.isSelected = false
    }

    function drawFlows(ctx){

        for(var i=0; i < flowConnections.length; i++){

            var item = flowConnections[i]

            var startPoint = item.attachsource.center()
            var endPoint = item.attachtarget.center()
            var fromx = startPoint.x
            var fromy = startPoint.y
            var tox = endPoint.x
            var toy = endPoint.y
            var headlen = 15;   // length of head in pixels
            var angle = Math.atan2(toy-fromy,tox-fromx);

            ctx.beginPath();
            ctx.moveTo(fromx, fromy);
            ctx.lineTo(tox, toy);
            ctx.lineTo(tox-headlen*Math.cos(angle-Math.PI/6),toy-headlen*Math.sin(angle-Math.PI/6));
            ctx.moveTo(tox, toy);
            ctx.lineTo(tox-headlen*Math.cos(angle+Math.PI/6),toy-headlen*Math.sin(angle+Math.PI/6));

            if(item.isSelected)
            {
                ctx.strokeStyle = "red"
                ctx.lineWidth = 3
            }else {
                ctx.strokeStyle = "blue"
                ctx.lineWidth = 1
            }

            ctx.stroke()

            if ( item.flowPoints){
                var p1 = item.flowPoints.fromParameterIndex
                var p2 = item.flowPoints.toParameterIndex
                var xcenter = (fromx + tox) / 2
                var ycenter = (fromy + toy) / 2
                var label = p1 + " > "+  p2
                ctx.fillStyle="#D8F6CE";
                ctx.fillRect(xcenter - 15 ,ycenter - 10,35,15);
                ctx.fillStyle = "#000";
                ctx.fillText(label, xcenter-10, ycenter + 2);
            }
        }
    }

    function setFlow(target, attachRect, flowPoints){
        if(workflow.hasValueDefined(target.itemid, flowPoints.toParameterIndex))
        {
            //TODO: Error gooien.
            return;
        }

        for(var i =0; i < flowConnections.length; ++i){
            if ( flowConnections[i].target == target)
                return // dont add duplicates
        }
        flowConnections.push({
            "target" : target,
            "source" :operationItem,
            "attachtarget" : attachRect,
            "attachsource" : selectedAttach,
            "flowPoints" : flowPoints,
            "isSelected" : false
        })
        workflow.addFlow(
            itemid,
            target.itemid,
            flowPoints,
            attachRect.index,
            selectedAttach.index
        )
        target.resetInputModel()
        wfCanvas.stopWorkingLine()
    }

    function attachFlow(target, attachRect){
        //If not connected to itself
        if ( wfCanvas.operationsList[wfCanvas.currentIndex] !== target){
            var flowPoints

            if( operation.isLegalFlow(wfCanvas.operationsList[wfCanvas.currentIndex].operation, target.operation))
            {
                if ( operation.needChoice(target.operation)){
                    wfCanvas.showAttachmentForm(true, target,attachRect)
                }
                else{
                    wfCanvas.operationsList[wfCanvas.currentIndex].setFlow(target,attachRect, null)
                }
            }
            else
            {
                wfCanvas.stopWorkingLine()
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
        anchors.topMargin: -2
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/4 - 5
    }
    AttachmentRectangle{
        id : att2
        index : 1
        anchors.top: operationItem.top
        anchors.topMargin: -2
        anchors.left : operationItem.left
        anchors.leftMargin: 3 * operationItem.width/4
    }
    AttachmentRectangle{
        id : att3
        index : 2
        anchors.top: operationItem.top
        anchors.topMargin: -2
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/2 - 5
    }

    AttachmentRectangle{
        id : att4
        index : 3
        anchors.bottom: operationItem.bottom
        anchors.bottomMargin: -2
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/4 - 5
    }
    AttachmentRectangle{
        id : att5
        index : 4
        anchors.bottom: operationItem.bottom
        anchors.bottomMargin: -2
        anchors.left : operationItem.left
        anchors.leftMargin: 3 * operationItem.width/4
    }

    AttachmentRectangle{
        id : att6
        index : 5
        anchors.bottom: operationItem.bottom
        anchors.bottomMargin: -2
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/2 - 5
    }

    AttachmentRectangle{
        id : att7
        index : 6
        anchors.left: operationItem.left
        anchors.leftMargin: 2
        anchors.top : operationItem.top
        anchors.topMargin: operationItem.height/2
    }

    AttachmentRectangle{
        id : att8
        index : 7
        anchors.right: operationItem.right
        anchors.rightMargin: 0
        anchors.top : operationItem.top
        anchors.topMargin: operationItem.height/2
    }
}

