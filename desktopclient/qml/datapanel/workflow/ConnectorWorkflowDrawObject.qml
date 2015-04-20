import QtQuick 2.0

BasicWorkflowDrawObject {

   readonly property string nameText: "Connector";

    property int defaultWidth : 5
    property int defaultHeight : 5

    width: defaultWidth
    height: defaultHeight

    property var startX
    property var startY
    property var endX
    property var endY

    function setCoordinates(newX, newY) {
        x = newX;
        y = newY;
    }

    function setStartCoordinate(newX, newY) {
        startX = newX;
        startY = newY;
        updateXY();
    }

    function setEndCoordinate(newX, newY) {
        endX = newX;
        endY = newY;
        updateXY();
    }

    function updateXY() {
        x = (startX + endX)/2
        y = (startY + endY)/2
    }

    function containsPosition(checkX, checkY) {
        return false;
    }

    function draw(ctx) {
        ctx.save();
        ctx.beginPath();
        ctx.moveTo(startX, startY);
        ctx.lineTo(endX, endY);
//         if (endX > startX) {
//        // draw arrow to the right
//        ctx.lineTo(endX - 2, endY - 5);
//        // draw the curve of the back
//        ctx.arcTo(endX, endY - 1, endX + 2, endY  - 5, 8);
//        // draw the bottom of the arrow head
//        ctx.lineTo(endX, endY);
//        // and make it draw
//        ctx.stroke();
//        ctx.fill();
//         } else {
//         }
        ctx.stroke();
        ctx.fill();


    }
}
