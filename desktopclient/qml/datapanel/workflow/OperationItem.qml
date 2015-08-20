import QtQuick 2.2
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
    Image {
        id : box
        anchors.fill: parent
        source : iconsource("operationitem.png")
    }
    Text{
        id : operationName
        anchors.top : box.top
        anchors.topMargin: 7
        width : box.width
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
        height : 30
        width : box.width
        clip : true
        x : 15
        model : operation ? operation.inParamNames : null
        delegate:
            Row {
            width : box.width
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
            Row {
            width : box.width
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
        for(var i =0; i < flowConnections.length; ++i){
            var item = flowConnections[i]
            var startPoint = item.attachsource.center()
            var endPoint = item.attachtarget.center()
            var fromx = startPoint.x
            var fromy = startPoint.y
            var tox = endPoint.x
            var toy = endPoint.y
            var headlen = 15;   // length of head in pixels
            var angle = Math.atan2(toy-fromy,tox-fromx);
            ctx.moveTo(fromx, fromy);
            ctx.lineTo(tox, toy);
            ctx.lineTo(tox-headlen*Math.cos(angle-Math.PI/6),toy-headlen*Math.sin(angle-Math.PI/6));
            ctx.moveTo(tox, toy);
            ctx.lineTo(tox-headlen*Math.cos(angle+Math.PI/6),toy-headlen*Math.sin(angle+Math.PI/6));
            ctx.stroke();
        }
    }

    function setFlow(target, attachRect){
        for(var i =0; i < flowConnections.length; ++i){
            if ( flowConnections[i].target == target && flowConnections[i].attachement == attachRect)
                return // dont add duplicates
        }

        flowConnections.push({"target" : target, "attachtarget": attachRect, "attachsource" : selectedAttach})
        wfCanvas.stopWorkingLine()
    }

    function attachFlow(target, attachRect){
        if ( wfCanvas.operationsList[wfCanvas.currentIndex] !== target){
            wfCanvas.operationsList[wfCanvas.currentIndex].setFlow(target,attachRect)
            wfCanvas.showAttachementForm(true)
            wfCanvas.canvasValid = false
        }
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

