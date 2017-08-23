import QtQuick 2.2
import QtQuick.Controls 1.0
import "../../../Global.js" as Global

ConditionItem {
    id : rangeItem
    property var selectedAttach
    property var attachementRectangles : []
    property bool junctionState : false

    type : "rangeitem"
    radius : 10
    testColor: "#78d3b5"
    operationColor: "#ecf9f4"
    junctionColor: "#9fe0ca"
    icon : "../../../images/loop.png"
    testItem: "RangeTests.qml"


    AttachmentRectangle{
        id : att2
        index : 0
        anchors.top: rangeItem.top
        x : rangeItem.width / 2 - 5
        anchors.topMargin: -10
        owner : rangeItem
        z : 2
        visible: type == "rangeitem"

    }

    AttachmentRectangle{
        id : att3
        index : 1
        anchors.top: rangeItem.top
        x : rangeItem.width / 2 - 5
        anchors.topMargin: 37
        owner : rangeItem
        z : 2
        visible: type == "rangeitem"

    }

    Rectangle {
        id : bottombuttons2
        anchors.bottom : rangeItem.bottom
       anchors.bottomMargin: 2
        width : parent.width - 8
        anchors.horizontalCenter: parent.horizontalCenter
        visible : type == "rangeitem"
        enabled:  visible
        height : enabled ? 24 : 0
        color : "#8FBC8F"
        border.width: 1
        border.color: "darkgrey"
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
                junctionState = true;
            }
        }

    }

    function handleClick(mx,my){
        if ( junctionState){
            var  p = wfCanvas.mapToItem(rangeItem, mx, my)
            console.debug(p.x, p.y)
            createJunction(p)
        }
    }

    function createJunction(point){
        var px=point.x,py=point.y

        var component = Qt.createComponent("RangeJunctionItem.qml");
        if (component.status === Component.Ready){
            var nodeid = workflow.addNode(0,{x : px, y:py,w:39, h:39,type:'rangejunctionnode', owner : itemid})
            workflow.addCondition2Junction(itemid, nodeid)
            currentItem = component.createObject(rangeItem, {"x": px, "y": py, "width":39, "height":39, "itemid" : nodeid, "scale": wfCanvas.scale});
            junctionsList.push(currentItem)
            currentItem.condition = rangeItem

            wfCanvas.canvasValid = false
            junctionState = false
        }
    }

    function attachFlow(target, attachRectTo){
        var fromIndex = 0
        var toIndex = 0
        var flowPoints = { "fromParameterIndex" : fromIndex, "toParameterIndex" : toIndex}
        currentItem.setFlow(target, selectedAttach.index, flowPoints,-1)
        if ( conditionTest){
            var value = "link=" + currentItem.itemid + ":" +fromIndex
            conditionTest.setTest(value)
        }
     }

    function attachementPoint(canvas, index){
         return index === 0 ? att2.center() : att3.center()
    }

    function parameterNames(typefilter, isOutput){
        var outputnames = ["currentitem"]
        return outputnames
    }

    function setFlow(target, attachRectIndex, flowPoints, testIndex){
       addFlowConnection(target, rangeItem, attachRectIndex, selectedAttach, flowPoints, -1, -1)
       if ( testIndex === -1){

            workflow.addFlow(
                        itemid,
                        target.itemid,
                        flowPoints.toParameterIndex,
                        flowPoints.fromParameterIndex,
                        selectedAttach.index,
                        attachRectIndex
                        )
        }
        wfCanvas.stopWorkingLine()

    }
    function addTestDataObject(objid, url, typename){
        if ( conditionTest){
            conditionTest.addTestOperation(objid, url, typename)
        }
    }

    function droppedOnTestPart( dx,dy) {
        return dx > x && dx < x + width && dy > y && dy < y + 28
    }

    function removeLinkTo(nodeid){
        for( var i=0; i < flowConnections.length; ++i){
            if ( flowConnections[i].target.itemid === nodeid){
                flowConnections.splice(i,1)
                break;
            }
        }

        for(var j=0; j < operationsList.length; ++j){
            operationsList[j].removeLinkTo(nodeid)
        }
        resetColors()
    }

    function storeRangeDefinition() {
        conditionTest.storeRangeDefinition()
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
        for( i=0; i < junctionsList.length; i++){
            junctionsList[i].drawFlows(ctx)
        }
    }

    Component.onCompleted: {
        attachementRectangles.push(att2)
        attachementRectangles.push(att3)
    }

}
