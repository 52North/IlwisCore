import QtQuick 2.0

BasicModellerObject {

    nameText: "Connector"

    property var endX
    property var endY

    function draw(ctx) {
        console.log(nameText, " drawn")
        ctx.beginPath();
        ctx.moveTo(x, y);
        endX = x + 100;
        endY = y;
        ctx.lineTo(x + 100, y);
        ctx.stroke();
    }

    function selected(checkX, checkY) {

        return false;
    }
}
