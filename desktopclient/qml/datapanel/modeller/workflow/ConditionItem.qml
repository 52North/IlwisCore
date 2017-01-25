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

    width: standardWidth
    height: standardHeight
    color: "transparent"
    border.width: 1
    border.color: workflow.isValidNode(itemid,"") ? "grey" : Global.errorColor
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

    }

    Rectangle {
        id: testRectangle
        property int detailsHeight : 0

        border.width: 1
        border.color: "grey"
        color: workflow.isValidNode(itemid,"tests") ? "#b3e6c9" : Global.errorColor
        height : 35 + detailsHeight
        width : parent.width - 8
        x : 4
        y: 4
        radius : 5
        z : 10

        DropArea {
            x : 2
            y : 2
            z: 1

            height : parent.height
            width : parent.width - 4
            enabled : true

            Text {
               anchors.centerIn: parent
               text : qsTr("Drop test operation(s) here")
               color : "grey"
            }
            onDropped: {
                addTestOperation(drag.source.ilwisobjectid)
            }

            ListView{
                id :testList
                property var tests : []
                anchors.fill: parent
                z : 2
                delegate: Column {
                    id : testPart
                    width : parent.width
                    height : 25
                    Row {
                        width : parent.width
                        height : 25
                        ComboBox{
                            width : 45
                            height : parent.height
                            model: [" ", "not"]
                        }

                        TextField{
                            id : testText
                            width : parent.width - 115
                            height : parent.height
                            readOnly: true
                            text : modelData.name2
                        }


                        Button {
                            width : 25
                            height : 25
                            Image {
                                width : 20
                                height : 20
                                anchors.centerIn: parent
                                source : "../../../images/zoomin20.png"
                            }
                            onClicked: {
                                if ( testDetails.height == 0){
                                    testDetails.model = null
                                    testDetails.model = workflow.getTestParameters(itemid, index)
                                    detailsBack.height = testDetails.model.length * 20
                                    testList.currentIndex = index
                                }else{
                                    for(var t=0; t < detailsBack.values.length; ++t)
                                        workflow.setTestValues(itemid, testList.currentIndex, t, detailsBack.values[t])
                                    detailsBack.values = []
                                    testDetails.model = null
                                    detailsBack.height = 0
                                    resetColors()

                                }
                            }
                        }

                        ComboBox{
                            width : 45
                            height : parent.height
                            model: [" ", "and", "or"]
                        }
                    }
                    Rectangle {
                        id : detailsBack
                        width : parent.width
                        height : 0
                        property var values : []
                        ListView {
                            id : testDetails
                            anchors.fill: parent
                            onModelChanged: {
                                detailsBack.height = model ? model.length * 20 : 0
                                testRectangle.detailsHeight = detailsBack.height
                            }

                            delegate : Item{
                                id : parmRow
                                width : parent.width
                                height : 20
                                Rectangle{
                                    id : attachDetail
                                    anchors.right : parmvalues.left
                                    anchors.rightMargin: 15
                                    width : 10
                                    height : 10
                                    border.width: 1
                                    MouseArea{
                                        anchors.fill : parent
                                        onClicked: {
                                            testDetails.currentIndex = index
                                            var datatype = workflow.testValueDataType(itemid, testList.currentIndex, index)
                                            attachTestFlow(type,index )
                                            att1.finishFlow(0,index)
                                        }
                                    }

                                }

                                Controls.TextEditLabelPair{
                                    id : parmvalues
                                    width : Math.max(0,parent.width - 20)
                                    labelWidth: 160
                                    labelText: modelData.label
                                    height : 20
                                    content : modelData.value
                                    onContentChanged: {
                                        detailsBack.values[index] = content
                                    }

                                }
                            }
                        }
                    }
                }
            }

        }
    }

    Rectangle {
        id: operationsRectangle

        anchors.topMargin: 4
        anchors.top: testRectangle.bottom
        border.width: 1
        border.color: "grey"
        color : workflow.isValidNode(itemid,"operations") ? "lightblue" : Global.errorColor
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
        height : 24
        color : "#d9f2e4"
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
        operation.condition = conditionItem
        resize()
    }

    function removeFromOperationList(operationIndex) {
    }


    function resize() {
        var newWidth = -1000000, newHeight = -1000000, operation, xChanged = false, yChanged = false, wChanged=false, hChanged=false;

        var newX = conditionItem.x, newY = conditionItem.y
        for (var i in operationsList) {
            operation = operationsList[i]

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
        if ( wChanged)
            conditionItem.width = newWidth + (padding * 2)
        if ( hChanged)
            conditionItem.height = newHeight + (padding * 2 + 10)
        if ( xChanged){
            conditionItem.x = newX
            operation.x = padding *2
        }
        if ( yChanged){
            conditionItem.y = newY
            operation.y = padding * 2 + 35
        }

    }

    function setTests() {
        testList.model = workflow.getTests(itemid)
    }

    function addTest(index, pre, operation, value, post,type) {
        if ( index === -1)
            index = testList.tests.length
        workflow.addTest2Condition(itemid,operation.id,pre, post)
        testList.model = null
        testList.model = workflow.getTests(itemid)
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
        wfCanvas.showTestForm(conditionItem, att1.index, testList.currentIndex, type, toIndex)
    }

    function changeBox(){
        workflow.changeBox(itemid, x,y,width, height)
    }

    function addTestOperation(operationid){
        var operation = operations.operation(operationid)
        if ( operation && operation.booleanOperation){
            addTest(-1, "", operation,"","","")
        }
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
        var value = "link=" + operationItemid + ":" +fromParam
        workflow.setTestValues(itemid, testList.currentIndex, toParam, value)
        testList.model = null
        testList.model = workflow.getTests(itemid)
    }

    function resetColors(){
        operationsRectangle.color = workflow.isValidNode(itemid, "operations") ? "#b3e6c9" : Global.errorColor
        testRectangle.color = workflow.isValidNode(itemid, "tests") ? "lightblue" : Global.errorColor
        conditionItem.border.color = workflow.isValidNode(itemid,"") ? "grey" : Global.errorColor
    }

    function resetInputModel(){
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
        for(j=0; j < operationsList.length; ++j){
            junctionsList[j].removeLinkTo(nodeid)
        }
        resetColors()
    }

    function addFlowConnection(targetItem, sourceItem, attachRectIndex,attachSource, flowPoints, testIndex, testParameter){
        workarea.addFlowConnection(flowConnections, targetItem, sourceItem, attachRectIndex,attachSource, flowPoints, testIndex, testParameter)

    }
}
