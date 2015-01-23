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
    property bool isDrag: false

    property real lastX
    property real lastY

    property BasicModellerObject currenCreateElement
    property BasicModellerObject currenSelectedElement

    Timer {
        interval: 30;
        running: true;
        repeat: true
        onTriggered: canvas.draw()
    }

    function addType(elem) {
        currenCreateElement = elem
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
        clearSelections();
        elements = [];
        clear();
        canvasValid = false;
    }

    function clearSelections() {
        currenSelectedElement = null;
        isDrag = false;
        canvasValid = false;
    }

    function checkForSelectedElement(mouseX, mouseY) {
        var l = elements.length;
        for (var i = l-1; i >= 0; i--) {
            if (elements[i].isSelected(mouseX, mouseY)) {
                currenSelectedElement = elements[i];
                canvasValid = false;
            }
        }
    }

    onWidthChanged: {
        canvasValid = false;

    }

    /*
      * click: onPressed, onReleased, onClicked
      * click'n'hold: onPressed, onPressAndHold, onReleased
      * click'n'hold and move: onPressed, onPositionChanged, onPressAndHold, onPositionChanged, onReleased
      */

    MouseArea {
        id: area
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: {
            console.log("onPressed")
            if (mouse.button == Qt.RightButton) {
                clearSelections();
            } else {
                 checkForSelectedElement(mouseX, mouseY);
            }
        }
        onPositionChanged: {
            console.log("onPositionChanged")
            if (pressed && currenSelectedElement != null && isDrag) {
                currenSelectedElement.setCoordinates(mouseX, mouseY);
                canvasValid = false;
            }
        }
        onPressAndHold: {
            console.log("onPressAndHold")
            if (currenSelectedElement != null && currenSelectedElement.containsPosition(mouseX, mouseY)) {
                isDrag = true;
            }
        }

        onClicked: {
            console.log("onClicked")
            if (currenCreateElement !== null) {
                currenCreateElement.setCoordinates(mouseX, mouseY)
                var t = elements
                t.push(currenCreateElement)
                elements = t
                currenCreateElement = null;
                canvasValid = false
            } else {

                if (currenSelectedElement != null) {
                    currenSelectedElement == null;
                    canvasValid = false;
                }
                checkForSelectedElement(mouseX, mouseY);
            }
        }
        onReleased: {
            console.log("onReleased")
                isDrag = false;
//            if (currenSelectedElement != null) {
//                currenSelectedElement.selected = false;
//                currenSelectedElement = null;
//            }
//            isDrag = false;
        }
        onDoubleClicked: {
            console.log("onDoubleClicked")
            // open metadata?
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
