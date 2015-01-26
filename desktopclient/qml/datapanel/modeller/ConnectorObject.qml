import QtQuick 2.0

BasicModellerObject {

   readonly property string nameText: "Connector";

    property int defaultWidth : 100
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
        // remove;
        setStartCoordiante(x-width/2, newY);
        setEndCoordinates(x+width/2, newY);
    }

    function setStartCoordiante(newX, newY) {
        startX = newX;
        startY = newY;
    }

    function setEndCoordinates(newX, newY) {
        endX = newX;
        endY = newY;
    }

    function draw(ctx) {
        ctx.save();
        ctx.beginPath();
        ctx.moveTo(startX, startY);
         if (selected) {
             //ctx.lineWidth = 2
             ctx.strokeStyle = "red"
             ctx.fillStyle = "red"
             ctx.lineTo(endY, endY);
         } else {
             ctx.lineTo(endX, endY);
         }

         if (endX > x) {
             // draw arrow to the right
             ctx.lineTo(endX - 5, endY - 2);
             // draw the curve of the back
             ctx.arcTo(endX2 - 1, endY, endX2 - 5, endY + 2,8);
             // draw the bottom of the arrow head
             ctx.lineTo(endX, endY);
             // and make it draw
             ctx.stroke();
             ctx.fill();
         } else {
             // draw arrow to the left
         }
         ctx.stroke();
         ctx.restore();

    }
}
