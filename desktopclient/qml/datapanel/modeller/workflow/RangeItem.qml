import QtQuick 2.0

ConditionItem {
    id : rangeItem
    property var selectedAttach
    property var attachementRectangles : []

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

    Component.onCompleted: {
        attachementRectangles.push(att2)
        attachementRectangles.push(att3)
    }

}
