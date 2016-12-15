import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../qml/controls" as Controls
import "../../../qml/Global.js" as Global


Item {
    id : createForm
    width : parent.width
    height : parent.height

    signal executeStandardization()

    property int thewidth : parent.width
    property int theheight : parent.height

    property int minX: 0
    property int maxX: 148607
    property int minY: 0
    property int maxY: 1

    /*ListModel {
        id: graphModel

        ListElement {
            minX: 0
            maxX: 148607
            minY: 0
            maxY: 1
        }
    }*/


    Canvas {
        id: graphCanvas
        width: 800
        height: 400


        // Repeaters and ListViews are too slow for drag functionality,
        // so we create the dragable points statically
        DragPoint {
            id: point0
            x: 10
            y: 150
            active: true
            fixedXX: true
        }

        DragPoint {
            id: point1
            x: 60
            y: 142
            active: true
            fixedXX: false
        }

        DragPoint {
            id: point2
            x: 110
            y: 165
            active: true
            fixedXX: false
        }

        DragPoint {
            id: point3
            x: 160
            y: 182
            active: true
            fixedXX: false
        }


        DragPoint {
            id: point4
            x: 210
            y: 150
            active: true
            fixedXX: false
        }

        DragPoint {
            id: point5
            x: 260
            y: 142
            active: true
            fixedXX: false
        }

        DragPoint {
            id: point6
            x: 310
            y: 165
            active: true
            fixedXX: false
        }

        DragPoint {
            id: point7
            x: 360
            y: 182
            active: true
            fixedXX: false
        }

        DragPoint {
            id: point8
            x: 410
            y: 182
            active: true
            fixedXX: true
        }


        function initializeCurve(ctx) {
            ctx.strokeStyle = "blue"
            ctx.beginPath()

            ctx.moveTo(point0.x, point0.y);
            ctx.lineTo(point1.x, point1.y);
            ctx.lineTo(point2.x, point2.y);
            ctx.lineTo(point3.x, point3.y);

            ctx.lineTo(point4.x, point4.y);
            ctx.lineTo(point5.x, point5.y);
            ctx.lineTo(point6.x, point6.y);
            ctx.lineTo(point7.x, point7.y);
            ctx.lineTo(point8.x, point8.y);

            ctx.stroke();
        }

        function drawAxis(ctx) {
            ctx.lineWidth = 2
            ctx.strokeStyle = "black"
            //ctx.fillStyle = "steelblue"
            // begin a new path to draw
            ctx.beginPath()
            // top-left start point
            ctx.moveTo(10,0) // YY
            // upper line
            ctx.lineTo(10,400) // O
            // right line
            ctx.lineTo(410,400) // XX
            ctx.stroke()
        }

        onPaint : {
            //console.log("onPaint called.....")
            var ctx = getContext("2d")

            /*ctx.translate(0, canvas.height)
            ctx.scale(1, -1)
            */
            drawAxis(ctx)
            initializeCurve(ctx)
        }

        Component.onCompleted: {

            point0.dragged.connect(repaint)
            point1.dragged.connect(repaint)
            point2.dragged.connect(repaint)
            point3.dragged.connect(repaint)

            point4.dragged.connect(repaint)
            point5.dragged.connect(repaint)
            point6.dragged.connect(repaint)
            point7.dragged.connect(repaint)

            point8.dragged.connect(repaint)

        }

        function repaint() {
            // test...
            if (point0.x > point1.x)
                point1.x = Math.min(point0.x, point8.x)
            if (point1.x > point2.x)
                point2.x = Math.min(point1.x, point8.x)
            if (point2.x > point3.x)
                point3.x = Math.min(point2.x, point8.x)
            if (point3.x > point4.x)
                point4.x = Math.min(point3.x, point8.x)
            if (point4.x > point5.x)
                point5.x = Math.min(point4.x, point8.x)
            if (point5.x > point6.x)
                point6.x = Math.min(point5.x, point8.x)
            if (point6.x > point7.x)
                point7.x = Math.min(point6.x, point8.x)

            if (point7.x > point8.x)
                point7.x = point8.x

            var ctx = getContext("2d");
            ctx.clearRect(0, 0, graphCanvas.width, graphCanvas.height);
            requestPaint()
        }
    }

    function toRealXX(x) {
        var scalerange = maxX - minX
        var graphXXrange = 410 - 10
        return (scalerange/graphXXrange) * (x-10) // 10 = XX origin
    }

    function toRealYY(y) {
        var scalerange = maxY - minY
        var graphYYrange = 400 - 0
        return 1 - ((scalerange/graphYYrange) * y)
    }

    function toViewportXX(x) {
        var scalerange = maxX - minX
        var graphXXrange = 410 - 10
        return 10 + ((graphXXrange/scalerange) * x) // 10 = XX origin
    }

    function toViewportYY(y) {
        var scalerange = maxY - minY
        var graphYYrange = 400 - 0
        return 400 - (graphYYrange/scalerange) * y
    }



    Rectangle {
        id: coords
        anchors.top: graphCanvas.bottom
        anchors.left: graphCanvas.left
        width: 800
        height: 40
        Controls.TextEditLabelPair {
             id: objCoordX0text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: parent.top
             anchors.left: parent.left
             content: parent.parent.toRealXX(point0.x)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("x0")
        }
        Controls.TextEditLabelPair {
             id: objCoordY0text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: parent.top
             anchors.left: objCoordX0text.right
             anchors.leftMargin: 10
             content: parent.parent.toRealYY(point0.y)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("y0")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point0.y = toViewportYY(parseFloat(content))
                 graphCanvas.repaint()
             }
        }

        ////

        Controls.TextEditLabelPair {
             id: objCoordX1text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordX0text.bottom
             anchors.left: parent.left
             content: parent.parent.toRealXX(point1.x)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("x1")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point1.x = toViewportXX(parseFloat(content))
                 graphCanvas.repaint()
             }
        }
        Controls.TextEditLabelPair {
             id: objCoordY1text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordY0text.bottom
             anchors.left: objCoordX1text.right
             anchors.leftMargin: 10
             content: parent.parent.toRealYY(point1.y)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("y1")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point1.y = toViewportYY(parseFloat(content))
                 graphCanvas.repaint()
             }
        }

        ////

        Controls.TextEditLabelPair {
             id: objCoordX2text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordX1text.bottom
             anchors.left: parent.left
             content: parent.parent.toRealXX(point2.x)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("x2")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point2.x = toViewportXX(parseFloat(content))
                 graphCanvas.repaint()
             }
        }
        Controls.TextEditLabelPair {
             id: objCoordY2text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordY1text.bottom
             anchors.left: objCoordX2text.right
             anchors.leftMargin: 10
             content: parent.parent.toRealYY(point2.y)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("y2")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point2.y = toViewportYY(parseFloat(content))
                 graphCanvas.repaint()
             }
        }

        ////

        Controls.TextEditLabelPair {
             id: objCoordX3text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordX2text.bottom
             anchors.left: parent.left
             content: parent.parent.toRealXX(point3.x)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("x3")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point3.x = toViewportXX(parseFloat(content))
                 graphCanvas.repaint()
             }
        }
        Controls.TextEditLabelPair {
             id: objCoordY3text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordY2text.bottom
             anchors.left: objCoordX3text.right
             anchors.leftMargin: 10
             content: parent.parent.toRealYY(point3.y)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("y3")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point3.y = toViewportYY(parseFloat(content))
                 graphCanvas.repaint()
             }
        }

        ////

        Controls.TextEditLabelPair {
             id: objCoordX4text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordX3text.bottom
             anchors.left: parent.left
             content: parent.parent.toRealXX(point4.x)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("x4")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point4.x = toViewportXX(parseFloat(content))
                 graphCanvas.repaint()
             }
        }
        Controls.TextEditLabelPair {
             id: objCoordY4text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordY3text.bottom
             anchors.left: objCoordX4text.right
             anchors.leftMargin: 10
             content: parent.parent.toRealYY(point4.y)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("y4")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point4.y = toViewportYY(parseFloat(content))
                 graphCanvas.repaint()
             }
        }

        ////

        Controls.TextEditLabelPair {
             id: objCoordX5text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordX4text.bottom
             anchors.left: parent.left
             content: parent.parent.toRealXX(point5.x)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("x5")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point5.x = toViewportXX(parseFloat(content))
                 graphCanvas.repaint()
             }
        }
        Controls.TextEditLabelPair {
             id: objCoordY5text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordY4text.bottom
             anchors.left: objCoordX5text.right
             anchors.leftMargin: 10
             content: parent.parent.toRealYY(point5.y)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("y5")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point5.y = toViewportYY(parseFloat(content))
                 graphCanvas.repaint()
             }
        }

        ////

        Controls.TextEditLabelPair {
             id: objCoordX6text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordX5text.bottom
             anchors.left: parent.left
             content: parent.parent.toRealXX(point6.x)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("x6")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point6.x = toViewportXX(parseFloat(content))
                 graphCanvas.repaint()
             }
        }
        Controls.TextEditLabelPair {
             id: objCoordY6text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordY5text.bottom
             anchors.left: objCoordX6text.right
             anchors.leftMargin: 10
             content: parent.parent.toRealYY(point6.y)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("y6")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point6.y = toViewportYY(parseFloat(content))
                 graphCanvas.repaint()
             }
        }

        ////

        Controls.TextEditLabelPair {
             id: objCoordX7text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordX6text.bottom
             anchors.left: parent.left
             content: parent.parent.toRealXX(point7.x)
             //color:"black"
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("x7")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point7.x = toViewportXX(parseFloat(content))
                 graphCanvas.repaint()
             }
        }
        Controls.TextEditLabelPair {
             id: objCoordY7text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordY6text.bottom
             anchors.left: objCoordX7text.right
             anchors.leftMargin: 10
             content: parent.parent.toRealYY(point7.y)
             //color:"black"
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("y7")
             onContentEdited: {
                 console.log("x7 changed to:"+ content)
                 point7.y = toViewportYY(parseFloat(content))
                 graphCanvas.repaint()
             }
        }

        ////

        Controls.TextEditLabelPair {
             id: objCoordX8text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordX7text.bottom
             anchors.left: parent.left
             content: parent.parent.toRealXX(point8.x)

             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("x8")
        }
        Controls.TextEditLabelPair {
             id: objCoordY8text
             width: 80
             height: 20
             labelWidth: 20
             anchors.top: objCoordY7text.bottom
             anchors.left: objCoordX8text.right
             anchors.leftMargin: 10
             content: parent.parent.toRealYY(point8.y)
             regexvalidator: /^-?\d*(\.\d*)?$/
             labelText: qsTr("y8")
             onContentEdited: {
                 console.log("y8 changed to:"+ content)
                 point8.y = toViewportYY(parseFloat(content))
                 graphCanvas.repaint()
             }
        }

        property var a1
        property var a2
        property var a3
        property var a4
        property var a5
        property var a6
        property var a7
        property var a8
        property var b1
        property var b2
        property var b3
        property var b4
        property var b5
        property var b6
        property var b7
        property var b8


        Button {
           text: "Apply"
           anchors.top: objCoordY8text.bottom
           anchors.left: objCoordY8text.left
           onClicked: {
                parent.a1 = (parent.parent.toRealYY(point1.y) - parent.parent.toRealYY(point0.y)) / (parent.parent.toRealXX(point1.x) - parent.parent.toRealXX(point0.x))
                parent.b1 = parent.parent.toRealYY(point0.y) - parent.a1 * parent.parent.toRealXX(point0.x)

                parent.a2 = (parent.parent.toRealYY(point2.y) - parent.parent.toRealYY(point1.y)) / (parent.parent.toRealXX(point2.x) - parent.parent.toRealXX(point1.x))
                parent.b2 = parent.parent.toRealYY(point1.y) - parent.a2 * parent.parent.toRealXX(point1.x)

                parent.a3 = (parent.parent.toRealYY(point3.y) - parent.parent.toRealYY(point2.y)) / (parent.parent.toRealXX(point3.x) - parent.parent.toRealXX(point2.x))
                parent.b3 = parent.parent.toRealYY(point2.y) - parent.a3 * parent.parent.toRealXX(point2.x)

               parent.a4 = (parent.parent.toRealYY(point4.y) - parent.parent.toRealYY(point3.y)) / (parent.parent.toRealXX(point4.x) - parent.parent.toRealXX(point3.x))
               parent.b4 = parent.parent.toRealYY(point3.y) - parent.a4 * parent.parent.toRealXX(point3.x)

               parent.a5 = (parent.parent.toRealYY(point5.y) - parent.parent.toRealYY(point5.y)) / (parent.parent.toRealXX(point5.x) - parent.parent.toRealXX(point4.x))
               parent.b5 = parent.parent.toRealYY(point4.y) - parent.a5 * parent.parent.toRealXX(point4.x)

               parent.a6 = (parent.parent.toRealYY(point6.y) - parent.parent.toRealYY(point6.y)) / (parent.parent.toRealXX(point6.x) - parent.parent.toRealXX(point5.x))
               parent.b6 = parent.parent.toRealYY(point5.y) - parent.a6 * parent.parent.toRealXX(point5.x)

               parent.a7 =(parent.parent.toRealYY(point7.y) - parent.parent.toRealYY(point7.y)) / (parent.parent.toRealXX(point7.x) - parent.parent.toRealXX(point6.x))
               parent.b7 = parent.parent.toRealYY(point6.y) - parent.a7 * parent.parent.toRealXX(point6.x)

               parent.a8 =(parent.parent.toRealYY(point8.y) - parent.parent.toRealYY(point8.y)) / (parent.parent.toRealXX(point8.x) - parent.parent.toRealXX(point7.x))
               parent.b8 = parent.parent.toRealYY(point7.y) - parent.a8 * parent.parent.toRealXX(point7.x)


               console.log( "a1=" + parent.a1 +
                           "\nb1=" +parent.b1 +
                           "\na2=" +parent.a2 +
                             "\nb2=" +parent.b2 +
                           "\na3=" + parent.a3 +
                                 "\nb3=" + parent.b3 +
                           "\na4=" + parent.a4 +
                                 "\nb4=" + parent.b4 +
                           "\na5=" +  parent.a5 +
                                 "\nb5=" + parent.b5 +
                           "\na6=" + parent.a6 +
                                 "\nb6=" + parent.b6 +
                           "\na7=" + parent.a7 +
                                 "\nb7=" + parent.b7 +
                           "\na8=" +  parent.a8 +
                                 "\nb8=" + parent.b8)


              console.log("x0=" + objCoordX0text.content +
                          "\nx1=" + objCoordX1text.content +
                          "\nx2=" + objCoordX2text.content +
                          "\nx3=" + objCoordX3text.content+
                          "\nx4=" + objCoordX4text.content+
                          "\nx5=" + objCoordX5text.content+
                          "\nx6=" + objCoordX6text.content+
                          "\nx7=" + objCoordX7text.content+
                          "\nx8=" + objCoordX8text.content)
           }
        }
    }
}
