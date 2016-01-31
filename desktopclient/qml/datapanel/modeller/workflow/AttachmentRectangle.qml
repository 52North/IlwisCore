import QtQuick 2.0

Rectangle {
    id : attachItem
    width: 8
    height: 8
    border.width: 1
    property int index
    opacity: (operationItem.isSelected ||wfCanvas.workingLineBegin.x !== -1) ? 1.0 : 0
    enabled : (operationItem.isSelected ||wfCanvas.workingLineBegin.x !== -1) ? true : false
    border.color : isSelected ? "red": "#A6B8C9"
    color : "transparent"
    property bool isSelected : false
    z : 1

    function center(){
        return mapToItem(wfCanvas, 4, 4);
    }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true

        onPressed: {
            operationItem.deselectAll()
            isSelected = true;
            selectedAttach = attachItem
            var newpoint = mapToItem(wfCanvas, 4, 4) // center of rectangle
            wfCanvas.workingLineBegin = Qt.point(newpoint.x, newpoint.y)
        }
        onReleased: {
            attachFlow(operationItem, attachItem)
        }
    }
}

