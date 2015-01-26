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
            ctx.lineWidth = 2;
            ctx.strokeStyle = "red";
            ctx.strokeRect(x-width/2, y-height/2, width, height);
        } else {
            ctx.rect(x-width/2, y-height/2, width, height);
        }
        ctx.stroke();
        ctx.restore();
        ctx.save();
        ctx.beginPath();
        if (dataSource !== "") {
            ctx.text(dataSource,(x - width/2) + 20, y);
        } else if (imagePath !== "") {
            //ctx.drawImage(imagePath, x - width/2 + 1, y + height/2 - 1, x + width/2 - 1, y + height/2 - 1);
            ctx.drawImage(imagePath, x-10, y-10);
        } else {
            ctx.text(nameText,(x - width/2) + 20, y);
        }
        ctx.stroke();
        ctx.restore();
    }

    function getInputConnector() {
        return Qt.point(x - width/2, y);
    }

    function getOutputConnector() {
        return Qt.point(x + width/2, y);
    }
}
