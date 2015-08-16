import QtQuick 2.0

Rectangle {
    width: 8
    height: 8
    border.width: 1
    opacity: (operationItem.isSelected ||wfCanvas.workingLineBegin.x !== -1) ? 1.0 : 0
    enabled : (operationItem.isSelected ||wfCanvas.workingLineBegin.x !== -1) ? true : false
    border.color : isSelected ? "red": "#A6B8C9"
    color : "transparent"
    property bool isSelected : false
    z : 1
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            operationItem.deselectAll()
            isSelected = true;
            var newpoint = mapToItem(wfCanvas, 4, 4) // center of rectangle
            wfCanvas.workingLineBegin = Qt.point(newpoint.x, newpoint.y)
        }
        onEntered: {
            var newpoint = mapToItem(wfCanvas, mouseX, mouseY)
            console.debug("entered", newpoint.x, newpoint.y)
        }
    }
}

