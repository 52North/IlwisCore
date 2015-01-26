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

    property BasicModellerObject currentCreateElement
    property BasicModellerObject currentSelectedElement
    property BasicModellerObject onPressedSelectedElement

    Timer {
        interval: 30;
        running: true;
        repeat: true
        onTriggered: canvas.draw()
    }

    function addType(elem) {
        currentCreateElement = elem
    }

    function invalidate() {
        validCanvas = false;
    }

    function draw(force){
        if (canvasValid == false || (force !== null && force)) {
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
         var l = elements.length;
         for (var i = l-1; i >= 0; i--) {
             elements[i].selected = false;
         }
        currentSelectedElement = null;
        isDrag = false;
        canvasValid = false;
    }

    function checkForSelectedElement(mouseX, mouseY) {
        var l = elements.length;
        var foundElement = false;
        for (var i = l-1; i >= 0; i--) {
            elements[i].selected = false;
            if (elements[i].isSelected(mouseX, mouseY)) {
                currentSelectedElement = elements[i];
                canvasValid = false;
                foundElement = true;
            }
        }
        return foundElement;
    }

    function createNewElement(mouseX, mouseY) {
        currentCreateElement.setCoordinates(mouseX, mouseY);
        if (currentSelectedElement != null){
            currentSelectedElement.selected = false;
        }
        currentSelectedElement = currentCreateElement;
        currentSelectedElement.selected = true;
        var t = elements;
        t.push(currentCreateElement);
        elements = t;
        currentCreateElement = null;
        canvasValid = false;
    }

    onWidthChanged: {
        // force re-draw if the ModellerPanel width has changed
        canvasValid = false;
    }

    onHeightChanged: {
        // force re-draw if the ModellerPanel height has changed
        canvas.draw(true);
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
            if (mouse.button == Qt.LeftButton) {
                if (checkForSelectedElement(mouseX, mouseY)) {
                    onPressedSelectedElement = currentSelectedElement;
                    isDrag = true;
                }
            }
        }

        onPositionChanged: {
            if (onPressedSelectedElement != null) {
                if (isDrag) {
                    currentSelectedElement.setCoordinates(mouseX, mouseY);
                    canvasValid = false;
                }
            }
        }

        onReleased: {
            isDrag = false;
            onPressedSelectedElement = null;
            if (mouse.button == Qt.LeftButton) {

            }
        }

        onClicked: {
            isDrag = false;
            if (mouse.button == Qt.LeftButton) {
                if (currentCreateElement !== null) {
//                    if (currentCreateElement.nameText == "Connector") {
//                        console.log(currentCreateElement.nameText);
//                        currentCreateElement = null;
//                    } else {
                        createNewElement(mouseX, mouseY);
//                    }
                }
            } else if (mouse.button == Qt.RightButton) {
                if (currentSelectedElement != null) {
                    clearSelections();
                }
            }
        }

        onDoubleClicked: {
            if (mouse.button == Qt.LeftButton) {
                // open metadata?
            }
        }
    }
}
