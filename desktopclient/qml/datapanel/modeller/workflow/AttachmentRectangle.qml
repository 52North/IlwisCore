import QtQuick 2.0

Rectangle {
    id : attachItem
    width: 8
    height: 8
    border.width: Math.max(2.0, 2.0 / wfCanvas.zoomScale)
    property int index
    property var owner
    opacity: (owner && (owner.isSelected ||wfCanvas.workingLineBegin.x !== -1)) ? 1.0 : 0
    enabled : (owner &&(owner.isSelected ||wfCanvas.workingLineBegin.x !== -1)) ? true : false
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
            owner.deselectAll()
            selectedAttach = attachItem
            isSelected = true;
        }
        onReleased: {
            finishFlow()

        }
    }
    function finishFlow(){
        var pnt = wfCanvas.workingLineBegin
        if ( pnt.x !== -1 && pnt.y !== -1){
            attachFlow(owner, index)
            wfCanvas.workingLineBegin = Qt.point(-1,-1)
            currentItem.isSelected = false
            wfCanvas.canvasValid = false

        }else {
            var newpoint = mapToItem(wfCanvas, 4, 4) // center of rectangle
            wfCanvas.workingLineBegin = Qt.point(newpoint.x, newpoint.y)
        }
    }

}

