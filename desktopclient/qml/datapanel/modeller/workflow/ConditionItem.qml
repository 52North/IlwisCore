import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls


Rectangle {
    id : conditionItem

    property int standardHeight: 180
    property int standardWidth: 360
    property int padding: 10
    property string type : "conditionitem"
    property var flowConnections: []
    property var operationsList : []
    //property var conditions : []
    property var junctionsList: []
    property var itemid
    property bool isSelected : false
    property string testColor : "#b6cee2"
    property string junctionColor : "#c9d7e8"
    property string operationColor : "#edf2f7"
    property string icon : "../../../images/choice20.png"
    property var conditionTest
    property string testItem: "ConditionTests.qml"

    width: standardWidth
    height: standardHeight
    color: "transparent"
    border.width: 1
    border.color: getBorderColor()
    transformOrigin: Item.TopLeft;
    radius : 5

    onXChanged: {
        changeBox()
    }
    onYChanged: {
        changeBox()
    }
    onWidthChanged: {
        changeBox()
    }
    onHeightChanged: {
        changeBox()
    }
    z : 4

    AttachmentRectangle{
        id : att1
        index : 0
        anchors.left: conditionItem.left
        anchors.leftMargin: -15
        anchors.top : conditionItem.top
        anchors.topMargin: 15
        owner : conditionItem
        z : 2
        visible: type == "condtionitem"

    }

    Loader {
        id : testRectangle
        height : 35 + (item ? item.detailsHeight : 0)
        width : parent.width - 8
        source : testItem
        onLoaded: {
            conditionTest = item
        }
    }

    Rectangle {
        id: operationsRectangle

        anchors.topMargin: 4
        anchors.top: testRectangle.bottom
        border.width: 1
        border.color: "grey"
        color :( workflow && workflow.isValidNode(itemid,"operations")) ? "lightblue" : Global.errorColor
        opacity : 0.1
        height: parent.height - testRectangle.height - 10 - bottombuttons.height
        width : parent.width - 8
        x : 4
        radius : 5
    }
    Rectangle {
        id : bottombuttons
        anchors.top : operationsRectangle.bottom
        width : parent.width - 8
        anchors.horizontalCenter: parent.horizontalCenter
        visible : type == "conditionitem"
        enabled:  visible
        height : enabled ? 24 : 0
        color : junctionColor
        border.width: 1
        border.color: "darkgrey"
        Button {
            width : 22
            height : 22
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 2
            Image {
                anchors.centerIn: parent
                width : 20
                height : 20
                source : "../../../images/junction.png"

            }
            onClicked: {
                createJunction(0)
            }
        }
        Button {
            width : 22
            height : 22
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 2
            Image {
                anchors.centerIn: parent
                width : 20
                height : 20
                source : "../../../images/junction.png"


            }
            onClicked: {
                createJunction(2)
            }
        }
        Button {
            width : 22
            height : 22
            anchors.verticalCenter: parent.verticalCenter
            x : parent.width / 2 - 11
            Image {
                anchors.centerIn: parent
                width : 20
                height : 20
                source : "../../../images/junction.png"

            }
            onClicked: {
                createJunction(1)
            }
        }

    }

    MouseArea {
        hoverEnabled: true
        anchors.bottom: parent.bottom
        width : parent.width
        height : 5

        onExited: {
            cursorShape = Qt.ArrowCursor
        }
    }


    function addToOperationList(operation) {
        operationsList.push(operation);
        if ( type == "condtionitem")
            operation.condition = conditionItem
        else
            operation.range = conditionItem
        resize()
    }

    function getBorderColor() {
        if (workflow){
            if ( workflow.lastOperationNode == itemid){
                return "blue"
            }
            return workflow.isValidNode(itemid,"") ? "grey" : Global.errorColor
        }
        return "white"
    }

    function removeFromOperationList(operationIndex) {
    }

    function resize() {
        var newWidth = -1000000, newHeight = -1000000, xChanged = false, yChanged = false, wChanged=false, hChanged=false;

        var newX = conditionItem.x, newY = conditionItem.y
        var node = null
        for (var i in operationsList) {
            var operation = operationsList[i]
            node = operation

            if ((operation.x + operation.width) > conditionItem.width) {
                newWidth = operation.x + operation.width
                wChanged = true
            }

            if ((operation.y  + operation.height) > conditionItem.height) {
               newHeight = operation.y  + operation.height
                hChanged = true
            }

            if (operation.x  < 0) {
               newWidth = Math.abs(operation.x) + conditionItem.width
               xChanged = wChanged = true
               newX = newX + operation.x - padding * 2
            }
            if (operation.y  < 0) {
               newHeight = Math.abs(operation.y) + conditionItem.height
               yChanged = hChanged = true
               newY = newY + operation.y - padding * 2
            }
        }
        if ( currentItem.type === "rangeitem"){
            for (i in junctionsList) {
                var junction = junctionsList[i]
                node = junction

                if ((junction.x + junction.width) > conditionItem.width) {
                    newWidth = junction.x + junction.width
                    wChanged = true
                }

                if ((junction.y  + junction.height) > conditionItem.height) {
                   newHeight = junction.y  + junction.height
                    hChanged = true
                }

                if (junction.x  < 0) {
                   newWidth = Math.abs(junction.x) + conditionItem.width
                   xChanged = wChanged = true
                   newX = newX + junction.x - padding * 2
                }
                if (junction.y  < 0) {
                   newHeight = Math.abs(junction.y) + conditionItem.height
                   yChanged = hChanged = true
                   newY = newY + junction.y - padding * 2
                }
            }
        }

        if ( wChanged)
            conditionItem.width = newWidth + (padding * 2)
        if ( hChanged)
            conditionItem.height = newHeight + (padding * 2 + 10)
        if ( xChanged){
            conditionItem.x = newX
            node.x = padding *2
        }
        if ( yChanged){
            conditionItem.y = newY
            node.y = padding * 2 + 35
        }

    }

    function setTests() {
        if ( conditionTest)
            conditionTest.setTests()
    }

    function addTest(index, pre, operation, value, post,type) {
        if ( conditionTest)
            conditionTest.addTest(index, pre, operation, value, post,type)
    }

    function inOperationList(my) {
        var yrelative = my - y
        return yrelative > operationsRectangle.y
    }

    function moveContent(dx, dy){
        for(var i=0; i < operationsList.length; ++i){
            var item = operationsList[i]
            item.x += dx
            item.y += dy
        }
        wfCanvas.canvasValid = false
        drawFlows(wfCanvas.ctx)
    }

    function inSideCondtion(centerY){
        var relY = centerY - y;
        return relY < testRectangle.height
    }

    function attachementPoint(canvas, index){
         return att1.center()
    }

     function attachFlow(target, attachRect){
         // no direct implementation as no one can directly do this
     }

    function attachTestFlow(type, toIndex){
        wfCanvas.showTestForm(conditionItem, att1.index, testRectangle.item.currentTestIndex, type, toIndex)
    }

    function changeBox(){
        workflow.changeBox(itemid, x,y,width, height)
    }

    function createJunction(side){
        var px,py, attach
        if (side === 1){
            px = conditionItem.x + conditionItem.width / 2 - 15
            py = conditionItem.y + height + 40
        }
        if (side === 0){
            px = conditionItem.x -80
            py = conditionItem.y + conditionItem.height / 2 - 15
        }
        if (side === 2){
            px = conditionItem.x + conditionItem.width + 80
            py = conditionItem.y + conditionItem.height / 2 - 15
        }
        var component = Qt.createComponent("JunctionItem.qml");
        if (component.status === Component.Ready){
            var nodeid = workflow.addNode(0,{x : px, y:py,w:39, h:39,type:'junctionnode'})
            workflow.addCondition2Junction(itemid, nodeid)
            currentItem = component.createObject(wfCanvas, {"x": px, "y": py, "width":39, "height":39, "itemid" : nodeid, "scale": wfCanvas.scale});
            junctionsList.push(currentItem)
            currentItem.condition = conditionItem

            wfCanvas.canvasValid = false
        }
    }

    function drawFlows(ctx){
        for(var i=0; i < junctionsList.length; i++){
            var item = junctionsList[i]
            var ep = Qt.point(conditionItem.x + width * wfCanvas.zoomScale/ 2.0, conditionItem.y + height * wfCanvas.zoomScale/2.0)
            var sp = item.center()

            if ( sp.x < conditionItem.x){
                ep.x = conditionItem.x
            }
            if ( sp.y < conditionItem.y){
                ep.y = conditionItem.y
            }
            if ( sp.x > (conditionItem.x + conditionItem.width* wfCanvas.zoomScale)){
                ep.x = conditionItem.x +conditionItem.width* wfCanvas.zoomScale
            }
            if ( sp.y > (conditionItem.y + conditionItem.height* wfCanvas.zoomScale)){
                ep.y = conditionItem.y + conditionItem.height* wfCanvas.zoomScale
            }

            Global.drawLine(wfCanvas, ctx, sp, ep, false, "lightgrey", 1)

            item.drawFlows(ctx)
        }
        for(i=0; i < flowConnections.length; i++){
            item = flowConnections[i]
            var index = item.attachtarget
            sp = item.attachsource.center()
            ep = item.target.attachementPoint(wfCanvas,index)

            var pt1 = Qt.point(sp.x, sp.y)
            var pt2 = Qt.point( ep.x, ep.y);

            Global.drawArrow(wfCanvas, ctx, pt1, pt2, item.isSelected)

        }
    }

    function setValue(operationItemid, fromParam, toParam){
        if(conditionTest)
            conditionTest.setValue(operationItemid, fromParam, toParam)
    }

    function resetColors(){
        operationsRectangle.color = workflow.isValidNode(itemid, "operations") ? operationColor : Global.errorColor
        if ( conditionTest)
            conditionTest.color = workflow.isValidNode(itemid, "tests") ? testColor : Global.errorColor
        conditionItem.border.color = workflow.isValidNode(itemid,"") ? "grey" : Global.errorColor
    }

    function resetInputModel(){
    }

    function clear() {
        for(var j=0; j < operationsList.length; ++j){
            operationsList[j].destroy()
        }
        for( j=0; j < junctionsList.length; ++j){
            junctionsList[j].destroy()
        }
        destroy()
    }

    function removeContent(){
       for(var j=0; j < operationsList.length; ++j){
           workarea.removeLinkTo(operationsList[j].itemid)
           workarea.removeItemFromList(workarea.operationsList, operationsList[j].itemid)
           operationsList[j].destroy()
       }
       for( j=0; j < junctionsList.length; ++j){
           workarea.removeLinkTo(junctionsList[j].itemid)
           junctionsList[j].destroy()
       }
    }

    function removeLinkTo(nodeid){
        for(var j=0; j < operationsList.length; ++j){
            operationsList[j].removeLinkTo(nodeid)
        }
        for(j=0; j < junctionsList.length; ++j){
            junctionsList[j].removeLinkTo(nodeid)
        }
        resetColors()
    }

    function addFlowConnection(targetItem, sourceItem, attachRectIndex,attachSource, flowPoints, testIndex, testParameter){
        workarea.addFlowConnection(flowConnections, targetItem, sourceItem, attachRectIndex,attachSource, flowPoints, testIndex, testParameter)

    }

    function addTestOperation(operationid){
        testRectangle.item.addTestOperation(operationid)
    }

    function deselectAll(){

    }

    function setLabel(value){
    }
}
