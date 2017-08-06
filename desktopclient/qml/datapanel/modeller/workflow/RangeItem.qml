import QtQuick 2.0

ConditionItem {
    id : rangeItem
    property var selectedAttach

    type : "rangeitem"
    radius : 10
    testColor: "#78d3b5"
    operationColor: "#ecf9f4"
    junctionColor: "#9fe0ca"
    icon : "../../../images/loop.png"
    testItem: "RangeTests.qml"

    AttachmentRectangle{
        id : att2
        index : 1
        anchors.top: rangeItem.top
        x : rangeItem.width / 2 - 5
        anchors.topMargin: -10
        owner : rangeItem
        z : 2
        visible: type == "rangeitem"

    }

    AttachmentRectangle{
        id : att3
        index : 2
        anchors.top: rangeItem.top
        x : rangeItem.width / 2 - 5
        anchors.topMargin: 45
        owner : rangeItem
        z : 2
        visible: type == "rangeitem"

    }

    function attachFlow(target, attachRectTo){
        console.debug("aap", target, rangeItem)
        var fromIndex = 0
        var toIndex = 1
        var flowPoints = { "fromParameterIndex" : fromIndex, "toParameterIndex" : toIndex}
        currentItem.setFlow(target, selectedAttach, flowPoints,-1)
     }

    function attachementPoint(canvas, index){
         return index === 1 ? att2.center() : att3.center()
    }


}
