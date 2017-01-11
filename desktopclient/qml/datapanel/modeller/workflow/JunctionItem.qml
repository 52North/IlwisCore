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
    property var linkedCondition
    property var flowConnections: []
    property var attachementRectangles : []

    transform: Scale { origin.x:  junctionItem.width /2; origin.y : junctionItem.height / 2; xScale : wfCanvas.zoomScale; yScale: wfCanvas.zoomScale}

    width : 39
    height : 39
    radius : 30
    color : Global.lightgreen
    border.width: 1
    border.color: Global.darkgreen

    onXChanged: {
        changeBox()
    }
    onYChanged: {
        changeBox()
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

    function attachFlow(target, attachRectTo){
        if ( currentItem !== target){
            wfCanvas.showAttachmentForm(target, attachRectTo, dataType)
//            var fromIndex = 0
//             var toIndex = 0
//            if ( 'condition' in currentItem){
//                if ( !currentItem.condition)
//                    toIndex = 1
//            }
//            var flowPoints = { "fromParameterIndex" : fromIndex, "toParameterIndex" : toIndex};
//            currentItem.setFlow(target, attachRectTo, flowPoints,-2)

            wfCanvas.canvasValid = false
        }
    }

    function attachJunctionFlow(att,toIndex){
        wfCanvas.showJunctionForm(junctionItem, att.index, dataType, toIndex)
    }

    function resetInputModel() {

    }

    function setFlow(target, attachRectIndex, flowPoints, testIndex){
        flowConnections.push({
                                 "target" : target,
                                 "source" :junctionItem,
                                 "attachtarget" : attachRectIndex,
                                 "attachsource" : selectedAttach,
                                 "flowPoints" : flowPoints,
                                 "isSelected" : false,
                                 "testindex" : -1,
                                 "testparameter" : -1 })
        workflow.addFlow(
                    itemid,
                    target.itemid,
                    flowPoints.toParameterIndex,
                    flowPoints.fromParameterIndex,
                    selectedAttach.index,
                    attachRectIndex
                    )
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
        return dummy
    }

    function containedInLinkedCondition(operationItem){
        if ( operationItem.condition)
            return operationItem.condition.itemid === linkedCondition.itemid
        return false
    }
    Component.onCompleted: {
        attachementRectangles.push(att1)
        attachementRectangles.push(att2)
        attachementRectangles.push(att3)
        attachementRectangles.push(att4)
    }

}
