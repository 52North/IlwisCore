import QtQuick 2.0

BasicWorkflowDrawObject {
    id: datasourceWorkflowDrawObject

    readonly property string nameText: "DataSource"

    property int defaultWidth : 100
    property int defaultHeight : 100

    width: defaultWidth
    height: defaultHeight


    function draw(ctx) {
        ctx.save();
        ctx.beginPath();
        if (!selected && getName() !== "not set") {
            ctx.rect(x-width/2, y-height/2, width, height);
        } else {
            if (selected) {
                ctx.lineWidth = 5;
                ctx.strokeStyle = "green";
            } else if (getName() === "not set") {
                ctx.lineWidth = 3;
                ctx.strokeStyle = "red";
            }
            ctx.strokeRect(x-width/2, y-height/2, width, height);
        }
        ctx.stroke();
        ctx.restore();
        ctx.save();
        ctx.beginPath();
        ctx.text(title ,(x - width/2) + 30, y - 10);
        var l = getName().length;
        if (l > 15) {
            ctx.text(getName().substring(0,15),(x - width/2) + 10, y + 10);
        } else {
            ctx.text(getName(),(x - width/2) + 10, y + 10);
        }
//        } else {
//            if (resourceId !== null) {
//                 ctx.text(resourceId,(x - width/2) + 20, y + 10);
//            } else{
//                 ctx.text(nameText,(x - width/2) + 20, y + 10);
//            }
//        }
        ctx.stroke();
        ctx.restore();
    }

    function getInputConnector() {
        return Qt.point(x, y - height/2);
    }

    function getOutputConnector() {
        return Qt.point(x, y + height/2);
    }
}
