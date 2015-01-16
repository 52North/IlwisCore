import QtQuick 2.0

Canvas {
    id:canvas
    property int paintX
    property int paintY
    property int count: 0
    property int lineWidth: 2
    property string drawColor: "black"
    property var ctx: getContext('2d')
    property variant elements: []
    property bool canvasValid: true

    property real lastX
    property real lastY

    property BasicModellerObject currenElement

    Timer {
        interval: 30;
        running: true;
        repeat: true
        onTriggered: canvas.draw()
    }

    function addType(elem) {
        currenElement = elem
    }

    function invalidate() {
        validCanvas = false;
    }

    function draw(){
        if (canvasValid == false) {
            clear(ctx);
            canvasValid = true
            var l = elements.length
            for (var i = 0; i < l; i++) {
                elements[i].draw(ctx);
            }
            canvas.requestPaint();

        }
    }

    function clear() {
        ctx.reset();
        ctx.clearRect(0, 0, width, height);
        ctx.stroke();
        canvas.requestPaint();
    }

    function clearModeller() {
        currenElement = null;
        elements = [];
        clear();
        canvasValid = false;
    }

    MouseArea {
        id: area
        anchors.fill: parent
        onPressed: {
            canvas.lastX = mouseX
            canvas.lastY = mouseY
        }
//        onPositionChanged: {
//            canvas.requestPaint()
//        }
        onClicked: {
            if (currenElement !== null) {
               currenElement.setCoordinates(mouseX, mouseY)
               var t = elements
               t.push(currenElement)
               elements = t
               currenElement = null;
               canvasValid = false
            } else {

            }
        }
    }

    //    MouseArea {
    //        id:mousearea
    //        hoverEnabled:true
    //        anchors.fill: parent
    //////        onClicked: drawPoint();
    //////        onPressed: drawPoint();
    //        onPositionChanged: {
    //            if (mousearea.pressed)
    //                drawLineSegment(paintX, paintY, mouseX, mouseY);
    //            paintX = mouseX;
    //            paintY = mouseY;
    //        }
    //////        onReleased: drawPoint();
    //    }
    //    function drawLineSegment(startX, startY, endX, endY) {
    //        ctx.beginPath();
    //        ctx.strokeStyle = drawColor
    //        ctx.lineWidth = lineWidth
    //        ctx.moveTo(startX, startY);
    //        ctx.lineTo(endX, endY);
    //        ctx.stroke();
    //        ctx.closePath();
    //    }
    //    function drawPoint() {
    //        ctx.lineWidth = lineWidth
    //        ctx.fillStyle = drawColor
    //        ctx.fillRect(mousearea.mouseX-10, mousearea.mouseY-10, 20, 20);
    //    }

    //    function zoomIn() {
    //        console.log("zoomInFunc");
    //        ctx.scale(1.5, 1.5);
    //    }

    //    function zoomOut() {
    //        console.log("zoomOutFunc");
    //        ctx.scale(0.5, 0.5);
    //    }

}
