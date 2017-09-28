import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import OperationModel 1.0
import "../../../Global.js" as Global

Rectangle {
    id : junctionItem
    property string type : "junctionitem"
    property var itemid;
    property bool isSelected : false
    property var selectedAttach
    property string dataType : ""
    property var condition
    property var flowConnections: []
    property var attachementRectangles : []

    width : 39
    height : 39
    radius : 30
    color : workflow.isValidNode(itemid,"") ? uicontext.lightColor : Global.errorColor
    border.width: 1
    border.color: uicontext.darkColor


    onXChanged: {
        changeBox()
    }
    onYChanged: {
        changeBox()
    }

    Text {
        id : lbl
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        text : ""

    }

    AttachmentRectangle{
        id : att1
        index : 0
        anchors.bottom: junctionItem.top
        x : junctionItem.width / 2 -4
        owner : junctionItem
    }
    AttachmentRectangle{
        id : att2
        index : 1
        anchors.right : junctionItem.left
        y : junctionItem.height / 2 -4
        owner : junctionItem

    }
    AttachmentRectangle{
        id : att3
        index : 2
        anchors.top: junctionItem.bottom
        x : junctionItem.width / 2 -4
        owner : junctionItem
    }

    AttachmentRectangle{
        id : att4
        index : 3
        y : junctionItem.height/2 -4
        anchors.left : junctionItem.right
        owner : junctionItem
    }

    function usableLink(sourceItemId, targetItemId){
        return workarea.workflow.usableLink(sourceItemId, targetItemId,-1,-1)
    }

    function attachFlow(target, attachRectTo){
        if ( currentItem === target)
            return false
        if ( usableLink(currentItem.itemid, target.itemid)){
            wfCanvas.showAttachmentForm(target, attachRectTo, dataType)
        }

        wfCanvas.canvasValid = false
    }

    function attachJunctionFlow(att,toIndex){
        wfCanvas.showJunctionForm(junctionItem, att.index, dataType, toIndex)
    }

    function resetColors(){
        color = workflow.isValidNode(itemid,"") ? uicontext.lightColor : Global.errorColor
    }

    function resetInputModel() {

    }

    function setFlow(target, attachRectIndex, flowPoints, testIndex){
        addFlowConnection(target, junctionItem,attachRectIndex,selectedAttach, flowPoints, -1,-1)
        workflow.addFlow(
                    itemid,
                    target.itemid,
                    flowPoints.toParameterIndex,
                    flowPoints.fromParameterIndex,
                    selectedAttach.index,
                    attachRectIndex
                    )
        workflow.createMetadata();
        workflowManager.updateRunForm()
        target.resetInputModel()
        wfCanvas.stopWorkingLine()
    }

    function drawFlows(ctx){
        for(var i=0; i < flowConnections.length; i++){
            var item = flowConnections[i]
            var index = item.attachtarget
            var sp = item.attachsource.center()
            var ep = item.target.attachementPoint(wfCanvas,index)
            var pt1 = Qt.point(sp.x, sp.y)
            var pt2 = Qt.point( ep.x, ep.y);

            Global.drawArrow(wfCanvas, ctx, pt1, pt2, item.isSelected)
        }
    }

    function attachementPoint(canvas, index){
        var at
        if ( index === 0)
            at = att1
        if ( index === 1)
            at = att2
        if ( index === 2)
            at = att3
        if ( index === 3)
           at = att4

        return at.center()
    }

    function center(){
        var p =  Qt.point(x + width/2, y + height/2)
        return p
    }

    function deselectAll(){
        att1.isSelected = att2.isSelected = att3.isSelected = att4.isSelected = false
    }

    function changeBox(){
        workflow.changeBox(itemid, x,y,width, height)
    }

    function parameterType(index, isOutput){
        return dataType
    }

    function parameterNames(typefilter, isOutput){
        var dummy = []
        // junction always has one output from whatever type
        dummy.push("dummy")
        if ( !isOutput)
            dummy.push("dummy2")
        return dummy
    }

    function containedInLinkedCondition(operationItem){
        if ( operationItem.condition)
            return operationItem.condition.itemid === condition.itemid
        return false
    }
    Component.onCompleted: {
        attachementRectangles.push(att1)
        attachementRectangles.push(att2)
        attachementRectangles.push(att3)
        attachementRectangles.push(att4)
    }

    function addFlowConnection(target, operationItem, attachRectIndex,attachSource, flowPoints, testIndex, testParameter){
        workarea.addFlowConnection(flowConnections, target, operationItem, attachRectIndex,attachSource, flowPoints, testIndex, testParameter)
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
        resetColors()
    }

    function setLabel(value){
        lbl.text = value
    }
}
