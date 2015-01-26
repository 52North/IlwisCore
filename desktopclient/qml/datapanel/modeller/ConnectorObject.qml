import QtQuick 2.0

BasicModellerObject {

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
        var d = ((startY-endY)*checkX + (endX-startX)*checkY + (startX*endY-endX*startY))/(Math.sqrt((endX-startX)*(endX-startX)+(endY-startY)*(endY-startY)));
        return (Math.sqrt(d*d)) <= defaultHeight;
    }

    function draw(ctx) {
        ctx.save();
        ctx.beginPath();
        ctx.moveTo(startX, startY);
         if (selected) {
             //ctx.lineWidth = 2
             ctx.strokeStyle = "red"
             ctx.fillStyle = "red"
             ctx.lineTo(endX, endY);
         } else {
             ctx.lineTo(endX, endY);
         }
//         if (endX > startX) {
//             // draw arrow to the right
//             ctx.lineTo(endX - 5, endY - 2);
//             // draw the curve of the back
//             ctx.arcTo(endX - 1, endY, endX - 5, endY + 2, 8);
//             // draw the bottom of the arrow head
//             ctx.lineTo(endX, endY);
//             // and make it draw
//             ctx.stroke();
//             ctx.fill();
//         } else {
//             // draw arrow to the left
//         }
         ctx.stroke();
         ctx.restore();

    }
}
