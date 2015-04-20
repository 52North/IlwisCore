import QtQuick 2.0

BasicWorkflowDrawObject {
    id: operationWorkflowDrawObject

    readonly property string nameText: "Operation"

    property var frame

    property int defaultWidth : 100
    property int defaultHeight : 100

    width: defaultWidth
    height: defaultHeight

//    Image {
//        id: icon
//        source: "../../world.svg"
//        //source: "../../images/workflow/operation.svg"
//        width: operationWorkflowDrawObject.width
//        height: operationWorkflowDrawObject.height
//    }
//
//    function scale(factor) {
//        icon.width = operationWorkflowDrawObject.width * factor
//        icon.height = operationWorkflowDrawObject.height * factor
//    }

    function draw(ctx) {
//        ctx.save();
//        ctx.beginPath();

//        ctx.drawImage(icon, x-width/2, y-height/2, 1500, 1500);
//        ctx.text("Leck mich fett!!1", (x - width/2) + 20, y);
//        ctx.stroke();

//        ctx.scale(10,10)

        ctx.save();
        ctx.beginPath();
        if (selected) {
            ctx.lineWidth = 5;
            ctx.strokeStyle = "green";
        } else {
            if (getName() === "not set") {
                ctx.lineWidth = 3;
                ctx.strokeStyle = "red";
            }
        }

        ctx.arc(x, y, width/2, 0, Math.PI*2, true);
        ctx.closePath();
        ctx.stroke();
        ctx.restore();
        ctx.save();
        ctx.beginPath();
        if (getName() !== null) {
            var l = getName().length;
            if (l > 15) {
                ctx.text(getName().substring(0,15),(x - width/4), y);
            } else {
                ctx.text(getName(),(x - width/4), y);
            }
        } else {
            if (resourceId !== null) {
                 ctx.text(resourceId,(x - width/4) + 20, y);
            } else{
                 ctx.text(nameText,(x - width/4) + 20, y);
            }
        }
        ctx.stroke();
        ctx.restore();
    }

    function getInputConnector() {
        return Qt.point(x, y - height/2);
    }

    function getOutputConnector() {
        return Qt.point(x, y + height/2);
    }

    function updateParentChilds() {
        if (name === frame.formTitle) {
            var result = frame.currentAppForm.formresult;
            var resultArray = result.split("|");
            var l = resultArray.length
            if (hasParentObjects()) {
                for(var i = 0; i < l-1; i++) {
                    if (parentObjects[i] !== null && parentObjects[i].hasParentObjects()) {
                        parentObjects[i].parentObjects[0].name = resultArray[i];
                    }
                }
            }
            if (hasChildObject()) {
                if (childObject.hasChildObject()) {
                    childObject.childObject.name = resultArray[l-1];
                }
            }
        }
    }

    onFrameChanged: {
        frame.formResultChanged.connect(updateParentChilds);
    }
}

