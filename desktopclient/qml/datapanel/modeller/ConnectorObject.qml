import QtQuick 2.0

BasicModellerObject {

    nameText: "Connector"

    property int defaultWidth : 100
    property int defaultHeight : 5

    width: defaultWidth
    height: defaultHeight

    property var endX
    property var endY

//    function isSelected(checkX, checkY) {
//        selected = checkX > (x - width) && checkX < (x + width) && checkY > (y - height) && checkY < (y + height)
//        return selected;
//    }

    function draw(ctx) {
        ctx.save();
        ctx.beginPath();
        ctx.moveTo(x-width/2, y);
        endX = x + width;
        endY = y;
         if (selected) {
             ctx.lineWidth = 2
             ctx.strokeStyle = "red"
             ctx.lineTo(x + width/2, y);
         } else {
             ctx.lineTo(x + width/2, y);
         }
         if (endX > x) {
             // draw arrow to the right
         } else {
             // draw arrow to the left
         }

        ctx.stroke();
        ctx.restore();
    }
}
