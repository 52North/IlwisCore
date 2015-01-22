import QtQuick 2.0

BasicModellerObject {

    nameText: "DataSource"

    property int defaultWidth : 100
    property int defaultHeight : 100

    width: defaultWidth
    height: defaultHeight


    function draw(ctx) {
        ctx.save();
        ctx.beginPath();
        if (selected) {
            ctx.lineWidth = 2
            ctx.strokeStyle = "red"
            ctx.strokeRect(x, y, width, height);
        } else {
            ctx.rect(x, y, width, height)
        }
        ctx.stroke();
        ctx.restore();
        ctx.save();
        ctx.beginPath();
        ctx.text(nameText ,x + 20, y + height/2);
        ctx.stroke();
        ctx.restore();
    }
}
