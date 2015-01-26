import QtQuick 2.0

BasicModellerObject {

    readonly property string nameText: "DataSource"

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
            ctx.strokeRect(x-width/2, y-height/2, width, height);
        } else {
            ctx.rect(x-width/2, y-height/2, width, height);
        }
        ctx.stroke();
        ctx.restore();
        ctx.save();
        ctx.beginPath();
        ctx.text(nameText,(x - width/2) + 20, y);
        ctx.stroke();
        ctx.restore();
    }
}
