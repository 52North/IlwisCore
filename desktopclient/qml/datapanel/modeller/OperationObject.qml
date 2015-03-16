import QtQuick 2.0

BasicModellerObject {
    id: operationObject

    readonly property string nameText: "Operation"

    property int defaultWidth : 50
    property int defaultHeight : 50

    width: defaultWidth
    height: defaultHeight

    Image {
        id: icon
        source: "../../world.svg"
        //source: "../../images/workflow/operation.svg"
        width: operationObject.width
        height: operationObject.height
    }

    function scale(factor) {
        icon.width = operationObject.width * factor
        icon.height = operationObject.height * factor
    }

    function draw(ctx) {
        ctx.save();
        ctx.beginPath();

        ctx.drawImage(icon, x-width/2, y-height/2, 1500, 1500);
        ctx.text("Leck mich fett!!1", (x - width/2) + 20, y);
        ctx.stroke();

        ctx.scale(10,10)

        /*
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
        if (objectContainer !== null) {
            if (objectContainer.ilwisObjectId !== null) {
                ctx.text(objectContainer.ilwisObjectId,(x - width/2) + 20, y);
            } else if (objectContainer.dataSource !== "") {
                ctx.text(objectContainer.dataSource,(x - width/2) + 20, y);
            } else if (objectContainer.imagePath !== "") {
                //ctx.drawImage(imagePath, x - width/2 + 1, y + height/2 - 1, x + width/2 - 1, y + height/2 - 1);
                ctx.drawImage(objectContainer.imagePath, x-10, y-10);
            } else {
                ctx.text(nameText,(x - width/2) + 20, y);
            }
        } else {
            ctx.text(nameText,(x - width/2) + 20, y);
        }
        ctx.stroke();
        */


        ctx.restore();
    }

    function getInputConnector() {
        return Qt.point(x - width/2, y);
    }

    function getOutputConnector() {
        return Qt.point(x + width/2, y);
    }

}
