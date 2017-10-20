import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import OperationModel 1.0
import "../../../Global.js" as Global

Rectangle {
    id : operationItem
    property OperationModel operation
    property var flowConnections: []
    property var attachementRectangles : []
    property bool isSelected : false
    property var selectedAttach
    property var itemid
    property var condition : null
    property var range : null
    property string type : "operationitem"
    property alias background : box.source
    transformOrigin: Item.TopLeft
    state : "maximized"
    z : 4
    border.width: (workflow && workflow.isValidNode(itemid,"all")) ? 0 : 1
    border.color: (workflow && workflow.isValidNode(itemid,"all")) ? "transparent" : "red"



    color:"transparent"
    width: 240
    height: 130


    onXChanged: {
        changeBox()
        wfCanvas.canvasValid = false
    }
    onYChanged: {
        changeBox()
        wfCanvas.canvasValid = false
    }
    onWidthChanged: {
        changeBox()
    }
    onHeightChanged: {
        changeBox()
    }

    Image {
        id : box
        anchors.fill: parent
        source : workflow != null ? getBackground(workflow.lastOperationNode) : ""
    }

    Text{
        id : operationName
        anchors.top : box.top
        anchors.topMargin: 4
        width : box.width -10
        elide: Text.ElideMiddle
        font.pixelSize: 11
        color : (workflow && workflow.currentNode == itemid) ? "red" : "black"
        x : 15
        text : itemid + ". " + (operation ? label() : "??")
        font.bold : true
        MouseArea{
            anchors.fill: parent
            onClicked: {
                if ( contentItem.height != 0){
                    operationItem.state = "minimized"
                }
                else{
                    operationItem.state = "maximized"
                }
                workflow.collapsed(itemid, operationItem.state == "minimized")
            }
        }
    }
    Item {
        anchors.top : operationName.bottom
        anchors.topMargin: 4
        height : parent.height - operationName.height
        width : parent.width
        id : contentItem
        clip : true
        Text {
            font.pixelSize: 10
            id : labelInput
            x : 15
            text : qsTr("Input parameters")
            font.bold: true
        }

        ScrollView {
            id : operationInParameters
            anchors.top : labelInput.bottom
            anchors.topMargin: 3
            height : 40
            width : box.width - 20
            clip : true
            x : 15
            ListView{
                id : operationInParametersList
                anchors.fill: parent
                model : getInputNames()
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
                            height : 11
                            font.pixelSize: 10
                        }

                        Text {
                            text : modelData
                            width : parent.width - 30
                            height : 11
                            font.bold: (workflow && workflow.hasValueDefined(itemid, index)) ? false : true
                            color : textColor(itemid, index)
                            elide: Text.ElideMiddle
                        }
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
            anchors.bottom: contentItem.bottom
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
    }



    AttachmentRectangle{
        id : att1
        index : 0
        anchors.top: operationItem.top
        anchors.topMargin: -3
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/4 - 5
        owner : operationItem
    }
    AttachmentRectangle{
        id : att2
        index : 1
        anchors.top: operationItem.top
        anchors.topMargin: -3
        anchors.left : operationItem.left
        anchors.leftMargin: 3 * operationItem.width/4
        owner : operationItem
    }
    AttachmentRectangle{
        id : att3
        index : 2
        anchors.top: operationItem.top
        anchors.topMargin: -3
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/2 - 5
        owner : operationItem
    }

    AttachmentRectangle{
        id : att4
        index : 3
        anchors.bottom: operationItem.bottom
        anchors.bottomMargin: -3
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/4 - 5
        owner : operationItem
    }
    AttachmentRectangle{
        id : att5
        index : 4
        anchors.bottom: operationItem.bottom
        anchors.bottomMargin: -3
        anchors.left : operationItem.left
        anchors.leftMargin: 3 * operationItem.width/4
        owner : operationItem
    }

    AttachmentRectangle{
        id : att6
        index : 5
        anchors.bottom: operationItem.bottom
        anchors.bottomMargin: -3
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/2 - 5
        owner : operationItem
    }

    AttachmentRectangle{
        id : att7
        index : 6
        anchors.left: operationItem.left
        anchors.leftMargin: -3
        anchors.top : operationItem.top
        anchors.topMargin: operationItem.height/2
        owner : operationItem
    }

    AttachmentRectangle{
        id : att8
        index : 7
        anchors.right: operationItem.right
        anchors.rightMargin: 3
        anchors.top : operationItem.top
        anchors.topMargin: operationItem.height/2
        owner : operationItem
    }

    Component.onCompleted: {
        attachementRectangles.push(att1)
        attachementRectangles.push(att2)
        attachementRectangles.push(att3)
        attachementRectangles.push(att4)
        attachementRectangles.push(att5)
        attachementRectangles.push(att6)
        attachementRectangles.push(att7)
        attachementRectangles.push(att8)
    }

    states: [
        State { name: "maximized"

            PropertyChanges {
                target: operationItem
                height : 130
            }
            PropertyChanges {
                target: contentItem
                height : 100
            }
        },
        State {
            name : "minimized"
            PropertyChanges {
                target: operationItem
                height : 30
            }
            PropertyChanges {
                target: contentItem
                height : 0
            }

        }
    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "height"; duration : 300 ; easing.type: Easing.InOutCubic }

            onRunningChanged: {
                wfCanvas.canvasValid = false
            }
        }
    ]
    function resetInputNames(){
        operationInParametersList.model = null
        operationInParametersList.model = getInputNames()
    }

    function getInputNames(){
        var lst = []
        if ( workflow){
            var node = workflow.getNode(itemid);

            if ( node){
                var parms = node["parameters"]

                if ( parms){
                    for(var i=0; i < parms.length; ++i){
                        lst.push(parms[i].label)
                    }
                }
            }
        }
        return lst
    }

    function label(){
        if ( workflow ){
            var node = workflow.getNode(itemid)
            return node.label
        }
    }

    function setLabel(value){
        operationName.text =  itemid + ". " + value
    }

    function textColor(nodeid, parmIndex){
        if (!workflow)
            return "white"

        var node = workflow.getNode(nodeid)
        if (node){
            var parms = node.parameters
            if ( parms && parmIndex < parms.length ){
                var parmstate = parms[parmIndex].state
                if ( parmstate === 'fixed')
                    return "green"
                if ( parmstate === 'calculated')
                    return "darkslategray"
            }
        }
        return condition ? "red": "blue"

    }


    function getBackground(lastitem) {
        var isSelected = false
        if (operation) {
            if ( operation.typeName == "workflow"){
                if ( isSelected){
                    return iconsource("workflowitemSelected.png")
                }
                return iconsource("workflowitem.png")
            }
        }
        if ( isSelected){
            if ( itemid == lastitem)
                return iconsource("operationitemSelectedStep.png")
            return iconsource("operationitemSelected.png")
        }else {
            if ( itemid == lastitem)
                return iconsource("operationitemStep.png")
            if ( workflow.currentNode == itemid){
                return iconsource("operationitemSelectedStep.png")
            }
            return iconsource("operationitem.png")
        }
    }

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../../../images/" + name
        return iconP
    }

    function drawFlows(ctx){
        if(!workflow)
            return

        for(var i=0; i < flowConnections.length; i++){
            var flow = flowConnections[i]
            var index = flow.attachtarget
            var sp = flow.attachsource.center()
            var ep = flow.target.attachementPoint(wfCanvas,index)
            var pt1 = Qt.point(sp.x, sp.y)
            var pt2 = Qt.point( ep.x, ep.y);

            Global.drawArrow(wfCanvas, ctx, pt1, pt2, flow.isSelected)

            if ( flow.flowPoints){
                var label = ""
                var nodetype = workflow.getNodeType(flow.target.itemid)
                if ( nodetype === "operationnode"){
                    label = workflow.getFlowLabel(flow.target.itemid, flow.flowPoints.toParameterIndex)
                }else if ( nodetype === "junctionnode"){
                    label = flow.flowPoints.toParameterIndex.toString()
                }
                drawInfoBox(ctx, pt1,pt2, label)
            }
        }
    }

    function drawInfoBox(ctx, pt1, pt2, label ){
        var fromx = pt1.x
        var fromy = pt1.y
        var tox = pt2.x
        var toy = pt2.y
        var xcenter = (fromx + tox) / 2
        var ycenter = (fromy + toy) / 2
        ctx.fillStyle="#D8F6CE";
        var sz = ctx.measureText(label)
        ctx.fillRect(xcenter - 15 ,ycenter - 10,sz.width + 4,15);
        ctx.fillStyle = "#000";
        ctx.fillText(label, xcenter-10, ycenter + 2);
    }


    function setFlow(target, attachRectIndex, flowPoints, testIndex){
        addFlowConnection(target, operationItem, attachRectIndex, selectedAttach, flowPoints, -1, -1)
       if ( testIndex === -1){
            workflow.addFlow(
                        itemid,
                        target.itemid,
                        flowPoints.toParameterIndex,
                        flowPoints.fromParameterIndex,
                        selectedAttach.index,
                        attachRectIndex
                        )
        }else if (testIndex === -2) {
            var truecase = target.containedInLinkedCondition(operationItem)
            workflow.addJunctionFlows(target.itemid, itemid,flowPoints.fromParameterIndex,selectedAttach.index,attachRectIndex,truecase)
        }else {
            workflow.addConditionFlow(
                        itemid,
                        target.itemid,
                        testIndex,
                        flowPoints.toParameterIndex,
                        flowPoints.fromParameterIndex,
                        selectedAttach.index,
                        attachRectIndex
                        )
        }
        workflow.createMetadata();
        workflowManager.updateRunForm()
        target.resetInputModel()
        wfCanvas.stopWorkingLine()

    }

    function usableLink(){
        // we only me draw links from items within the same condition
        // all links to the outside must use the junction
        if ( currentItem && (currentItem.type == "operationitem" || currentItem.type == "rangejunctionitem")){
            if ( currentItem.condition){
                if ( condition){
                    if ( currentItem.condition.itemid !== condition.itemid)
                        return false
                }else if ( range){
                    if ( currentItem.condition.itemid !== range.itemid)
                        return false
                }else
                    return false
            }
            return true
        }
        var maxcount = workarea.workflow.freeInputParameterCount(itemid)
        if ( maxcount === 0){
            return false
        }
        return true
    }

    function checkValidityFlow(nodeFrom, nodeTo){

        var type = nodeTo.parameterType(0,false)
        var outParameterNames = nodeFrom.parameterNames(type,true)
        return outParameterNames.length > 0
    }

    function attachFlow(target, attachRectTo){
        //If not connected to itself
        if ( currentItem !== target){
            if ( usableLink()){
                if (operation.needChoice(target.operation)) {
                    wfCanvas.showAttachmentForm(target, attachRectTo,"")
                } else {
                     if ( checkValidityFlow(workarea.currentItem, target)){
                        var fromIndex = 0
                        var toIndex = 0
                        var flowPoints = { "fromParameterIndex" : fromIndex, "toParameterIndex" : toIndex};
                        currentItem.setFlow(target, attachRectTo, flowPoints,-1)
                        target.resetColors()
                    }

                }
                if ( condition)
                    condition.resetColors()
            }
        }
    }

    function resetInputModel(){
        operationInParametersList.model = null
        operationInParametersList.model = operation.inParamNames
    }

    function deselectAll(){
        att1.isSelected = att2.isSelected = att3.isSelected = att4.isSelected = att5.isSelected = att6.isSelected = att7.isSelected = att8.isSelected = false
    }

    function attachmentRectangle(index){
        return attachementRectangles[index]
    }

    function attachementPoint(canvas, index){
        return attachmentRectangle(index).center();
    }

    function changeBox(){
        workflow.changeBox(itemid, x,y,width, height)
    }

    function parameterType(index, isOutput){
        if ( isOutput)
            return operation.outputparameterType(index)
        return operation.inputparameterType(index)
    }

    function parameterNames(typefilter, isOutput){
        return operation.parameterIndexes(typefilter,isOutput)
    }
    function removeLinkTo(nodeid){
        var i=0
        var len = flowConnections.length
        while(i < len){
            if ( flowConnections[i].target.itemid == nodeid){
                flowConnections.splice(i,1)
                --len
            }else
                ++i
        }
    }
    function addFlowConnection(targetItem, sourceItem, attachRectIndex,attachSource, flowPoints, testIndex, testParameter){
        workarea.addFlowConnection(flowConnections, targetItem, sourceItem, attachRectIndex,attachSource, flowPoints, testIndex, testParameter)
    }

    function resetColors(){
        if ( condition){
            var valid = workflow.isValidNode(itemid,"all")
            border.width =  valid ? 0 : 1
            border.color = valid ? "transparent" : "red"
            condition.resetColors()
        }
    }
}
