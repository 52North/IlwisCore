import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../qml/controls" as Controls
import "../../../qml/Global.js" as Global


Column {
    id : createForm
    width : parent.width
    spacing: 1

    signal executeStandardization()

    property int minX: 0
    property int maxX: 148607
    property int minY: 0
    property int maxY: 1

    Canvas {
        id: graphCanvas
        width: parent.width
        height: 200
        property int axisOffset : 10

        // Repeaters and ListViews are too slow for drag functionality,
        // so we create the dragable points statically
        DragPoint {
            id: point0
            x: graphCanvas.axisOffset
            y: graphCanvas.height - graphCanvas.axisOffset
            active: true
            fixedXX: true
        }

        DragPoint {
            id: point1
            x: Math.round(graphCanvas.axisOffset + (graphCanvas.width - graphCanvas.axisOffset) / 8.0)
            y: Math.round(graphCanvas.height - graphCanvas.axisOffset - (graphCanvas.height - graphCanvas.axisOffset) / 8.0)
            active: true
            fixedXX: false
        }

        DragPoint {
            id: point2
            x: Math.round(graphCanvas.axisOffset + 2.0 * (graphCanvas.width - graphCanvas.axisOffset) / 8.0)
            y: Math.round(graphCanvas.height - graphCanvas.axisOffset - 2.0 * (graphCanvas.height - graphCanvas.axisOffset) / 8.0)
            active: true
            fixedXX: false
        }

        DragPoint {
            id: point3
            x: Math.round(graphCanvas.axisOffset + 3.0 * (graphCanvas.width - graphCanvas.axisOffset) / 8.0)
            y: Math.round(graphCanvas.height - graphCanvas.axisOffset - 3.0 * (graphCanvas.height - graphCanvas.axisOffset) / 8.0)
            active: true
            fixedXX: false
        }

        DragPoint {
            id: point4
            x: Math.round(graphCanvas.axisOffset + (graphCanvas.width - graphCanvas.axisOffset) / 2.0)
            y: Math.round(graphCanvas.height - graphCanvas.axisOffset - 4.0 * (graphCanvas.height - graphCanvas.axisOffset) / 8.0)
            active: true
            fixedXX: false
        }

        DragPoint {
            id: point5
            x: Math.round(graphCanvas.axisOffset + 5.0 * (graphCanvas.width - graphCanvas.axisOffset) / 8.0)
            y: Math.round(graphCanvas.height - graphCanvas.axisOffset - 5.0 * (graphCanvas.height - graphCanvas.axisOffset) / 8.0)
            active: true
            fixedXX: false
        }

        DragPoint {
            id: point6
            x: Math.round(graphCanvas.axisOffset + 6.0 * (graphCanvas.width - graphCanvas.axisOffset) / 8.0)
            y: Math.round(graphCanvas.height - graphCanvas.axisOffset - 6.0 * (graphCanvas.height - graphCanvas.axisOffset) / 8.0)
            active: true
            fixedXX: false
        }

        DragPoint {
            id: point7
            x: Math.round(graphCanvas.axisOffset + 7.0 * (graphCanvas.width - graphCanvas.axisOffset) / 8.0)
            y: Math.round(graphCanvas.height - graphCanvas.axisOffset - 7.0 * (graphCanvas.height - graphCanvas.axisOffset) / 8.0)
            active: true
            fixedXX: false
        }

        DragPoint {
            id: point8
            x: graphCanvas.width
            y: 0
            active: true
            fixedXX: true
        }

        function initializeCurve(ctx) {
            ctx.strokeStyle = "blue"
            ctx.beginPath()
            ctx.moveTo(0.5 + point0.x, 0.5 + point0.y) // +0.5 everywhere otherwise Canvas blurs the horizontal and vertical lines
            ctx.lineTo(0.5 + point1.x, 0.5 + point1.y)
            ctx.lineTo(0.5 + point2.x, 0.5 + point2.y)
            ctx.lineTo(0.5 + point3.x, 0.5 + point3.y)
            ctx.lineTo(0.5 + point4.x, 0.5 + point4.y)
            ctx.lineTo(0.5 + point5.x, 0.5 + point5.y)
            ctx.lineTo(0.5 + point6.x, 0.5 + point6.y)
            ctx.lineTo(0.5 + point7.x, 0.5 + point7.y)
            ctx.lineTo(0.5 + point8.x, 0.5 + point8.y)
            ctx.stroke()
        }

        function drawAxis(ctx) {
            ctx.lineWidth = 1.0
            ctx.strokeStyle = "black"
            ctx.beginPath()
            ctx.moveTo(0.5 + graphCanvas.axisOffset, 0.5) // top-left
            ctx.lineTo(0.5 + graphCanvas.axisOffset, 0.5 + graphCanvas.height - graphCanvas.axisOffset) // origin
            ctx.lineTo(0.5 + graphCanvas.width, 0.5 + graphCanvas.height - graphCanvas.axisOffset) // bottom-right
            ctx.stroke()
        }

        onPaint : {
            //console.log("onPaint called.....")
            var ctx = getContext("2d")

            /*ctx.translate(0, canvas.height)
            ctx.scale(1, -1)
            */
            ctx.fillStyle = "white"
            ctx.fillRect(0, 0, graphCanvas.width, graphCanvas.height)
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
            point0.x = Math.max(graphCanvas.axisOffset, Math.min(point0.x, graphCanvas.width))
            point1.x = Math.max(graphCanvas.axisOffset, Math.min(point1.x, graphCanvas.width))
            point2.x = Math.max(graphCanvas.axisOffset, Math.min(point2.x, graphCanvas.width))
            point3.x = Math.max(graphCanvas.axisOffset, Math.min(point3.x, graphCanvas.width))
            point4.x = Math.max(graphCanvas.axisOffset, Math.min(point4.x, graphCanvas.width))
            point5.x = Math.max(graphCanvas.axisOffset, Math.min(point5.x, graphCanvas.width))
            point6.x = Math.max(graphCanvas.axisOffset, Math.min(point6.x, graphCanvas.width))
            point7.x = Math.max(graphCanvas.axisOffset, Math.min(point7.x, graphCanvas.width))
            point8.x = Math.max(graphCanvas.axisOffset, Math.min(point8.x, graphCanvas.width))
            point0.y = Math.max(0, Math.min(point0.y, graphCanvas.height - graphCanvas.axisOffset))
            point1.y = Math.max(0, Math.min(point1.y, graphCanvas.height - graphCanvas.axisOffset))
            point2.y = Math.max(0, Math.min(point2.y, graphCanvas.height - graphCanvas.axisOffset))
            point3.y = Math.max(0, Math.min(point3.y, graphCanvas.height - graphCanvas.axisOffset))
            point4.y = Math.max(0, Math.min(point4.y, graphCanvas.height - graphCanvas.axisOffset))
            point5.y = Math.max(0, Math.min(point5.y, graphCanvas.height - graphCanvas.axisOffset))
            point6.y = Math.max(0, Math.min(point6.y, graphCanvas.height - graphCanvas.axisOffset))
            point7.y = Math.max(0, Math.min(point7.y, graphCanvas.height - graphCanvas.axisOffset))
            point8.y = Math.max(0, Math.min(point8.y, graphCanvas.height - graphCanvas.axisOffset))
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
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, graphCanvas.width, graphCanvas.height)
            requestPaint()
        }
    }

    function toRealXX(x) {
        var scalerange = maxX - minX
        var graphXXrange = graphCanvas.width
        return (scalerange/graphXXrange) * (x - graphCanvas.axisOffset) // axisOffset = XX origin
    }

    function toRealYY(y) {
        var scalerange = maxY - minY
        var graphYYrange = graphCanvas.height - graphCanvas.axisOffset
        return 1.0 - ((scalerange/graphYYrange) * y)
    }

    function toViewportXX(x) {
        var scalerange = maxX - minX
        var graphXXrange = graphCanvas.width
        return Math.round(graphCanvas.axisOffset + ((graphXXrange/scalerange) * x)) // axisOffset = XX origin
    }

    function toViewportYY(y) {
        var scalerange = maxY - minY
        var graphYYrange = graphCanvas.height - graphCanvas.axisOffset
        return Math.round(graphCanvas.height - (graphYYrange/scalerange) * y)
    }

    Row {
        Controls.TextEditLabelPair {
            id: objCoordX0text
            width: 100
            height: 20
            labelWidth: 20
            content: minX.toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x0")
            readOnly: true
        }
        Controls.TextEditLabelPair {
            id: objCoordY0text
            width: 80
            height: 20
            labelWidth: 20
            content: toRealYY(point0.y).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y0")
            onContentEdited: {
                console.log("y0 changed to:"+ content)
                point0.y = toViewportYY(parseFloat(content))
                graphCanvas.repaint()
            }
        }
    }

    ////

    Row {
        Controls.TextEditLabelPair {
            id: objCoordX1text
            width: 100
            height: 20
            labelWidth: 20
            content: toRealXX(point1.x).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x1")
            onContentEdited: {
                console.log("x1 changed to:"+ content)
                point1.x = toViewportXX(parseFloat(content))
                graphCanvas.repaint()
            }
        }
        Controls.TextEditLabelPair {
            id: objCoordY1text
            width: 80
            height: 20
            labelWidth: 20
            content: toRealYY(point1.y).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y1")
            onContentEdited: {
                console.log("y1 changed to:"+ content)
                point1.y = toViewportYY(parseFloat(content))
                graphCanvas.repaint()
            }
        }
    }

    ////

    Row {
        Controls.TextEditLabelPair {
            id: objCoordX2text
            width: 100
            height: 20
            labelWidth: 20
            content: toRealXX(point2.x).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x2")
            onContentEdited: {
                console.log("x2 changed to:"+ content)
                point2.x = toViewportXX(parseFloat(content))
                graphCanvas.repaint()
            }
        }
        Controls.TextEditLabelPair {
            id: objCoordY2text
            width: 80
            height: 20
            labelWidth: 20
            content: toRealYY(point2.y).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y2")
            onContentEdited: {
                console.log("y2 changed to:"+ content)
                point2.y = toViewportYY(parseFloat(content))
                graphCanvas.repaint()
            }
        }
    }

    ////

    Row {
        Controls.TextEditLabelPair {
            id: objCoordX3text
            width: 100
            height: 20
            labelWidth: 20
            content: toRealXX(point3.x).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x3")
            onContentEdited: {
                console.log("x3 changed to:"+ content)
                point3.x = toViewportXX(parseFloat(content))
                graphCanvas.repaint()
            }
        }
        Controls.TextEditLabelPair {
            id: objCoordY3text
            width: 80
            height: 20
            labelWidth: 20
            content: toRealYY(point3.y).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y3")
            onContentEdited: {
                console.log("y3 changed to:"+ content)
                point3.y = toViewportYY(parseFloat(content))
                graphCanvas.repaint()
            }
        }
    }

    ////

    Row {
        Controls.TextEditLabelPair {
            id: objCoordX4text
            width: 100
            height: 20
            labelWidth: 20
            content: toRealXX(point4.x).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x4")
            onContentEdited: {
                console.log("x4 changed to:"+ content)
                point4.x = toViewportXX(parseFloat(content))
                graphCanvas.repaint()
            }
        }
        Controls.TextEditLabelPair {
            id: objCoordY4text
            width: 80
            height: 20
            labelWidth: 20
            content: toRealYY(point4.y).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y4")
            onContentEdited: {
                console.log("y4 changed to:"+ content)
                point4.y = toViewportYY(parseFloat(content))
                graphCanvas.repaint()
            }
        }
    }

    ////

    Row {
        Controls.TextEditLabelPair {
            id: objCoordX5text
            width: 100
            height: 20
            labelWidth: 20
            content: toRealXX(point5.x).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x5")
            onContentEdited: {
                console.log("x5 changed to:"+ content)
                point5.x = toViewportXX(parseFloat(content))
                graphCanvas.repaint()
            }
        }
        Controls.TextEditLabelPair {
            id: objCoordY5text
            width: 80
            height: 20
            labelWidth: 20
            content: toRealYY(point5.y).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y5")
            onContentEdited: {
                console.log("y5 changed to:"+ content)
                point5.y = toViewportYY(parseFloat(content))
                graphCanvas.repaint()
            }
        }
    }

    ////

    Row {
        Controls.TextEditLabelPair {
            id: objCoordX6text
            width: 100
            height: 20
            labelWidth: 20
            content: toRealXX(point6.x).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x6")
            onContentEdited: {
                console.log("x6 changed to:"+ content)
                point6.x = toViewportXX(parseFloat(content))
                graphCanvas.repaint()
            }
        }
        Controls.TextEditLabelPair {
            id: objCoordY6text
            width: 80
            height: 20
            labelWidth: 20
            content: toRealYY(point6.y).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y6")
            onContentEdited: {
                console.log("y6 changed to:"+ content)
                point6.y = toViewportYY(parseFloat(content))
                graphCanvas.repaint()
            }
        }
    }

    ////

    Row {
        Controls.TextEditLabelPair {
            id: objCoordX7text
            width: 100
            height: 20
            labelWidth: 20
            content: toRealXX(point7.x).toFixed(3).toString()
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
            content: toRealYY(point7.y).toFixed(3).toString()
            //color:"black"
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y7")
            onContentEdited: {
                console.log("y7 changed to:"+ content)
                point7.y = toViewportYY(parseFloat(content))
                graphCanvas.repaint()
            }
        }
    }

    ////

    Row {
        Controls.TextEditLabelPair {
            id: objCoordX8text
            width: 100
            height: 20
            labelWidth: 20
            content: maxX.toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x8")
            readOnly: true
        }
        Controls.TextEditLabelPair {
            id: objCoordY8text
            width: 80
            height: 20
            labelWidth: 20
            content: toRealYY(point8.y).toFixed(3).toString()
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y8")
            onContentEdited: {
                console.log("y8 changed to:"+ content)
                point8.y = toViewportYY(parseFloat(content))
                graphCanvas.repaint()
            }
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
        id: applyButton
        text: "Apply"
        onClicked: {
            parent.a1 = (toRealYY(point1.y) - toRealYY(point0.y)) / (toRealXX(point1.x) - toRealXX(point0.x))
            parent.b1 = toRealYY(point0.y) - parent.a1 * toRealXX(point0.x)

            parent.a2 = (toRealYY(point2.y) - toRealYY(point1.y)) / (toRealXX(point2.x) - toRealXX(point1.x))
            parent.b2 = toRealYY(point1.y) - parent.a2 * toRealXX(point1.x)

            parent.a3 = (toRealYY(point3.y) - toRealYY(point2.y)) / (toRealXX(point3.x) - toRealXX(point2.x))
            parent.b3 = toRealYY(point2.y) - parent.a3 * toRealXX(point2.x)

            parent.a4 = (toRealYY(point4.y) - toRealYY(point3.y)) / (toRealXX(point4.x) - toRealXX(point3.x))
            parent.b4 = toRealYY(point3.y) - parent.a4 * toRealXX(point3.x)

            parent.a5 = (toRealYY(point5.y) - toRealYY(point5.y)) / (toRealXX(point5.x) - toRealXX(point4.x))
            parent.b5 = toRealYY(point4.y) - parent.a5 * toRealXX(point4.x)

            parent.a6 = (toRealYY(point6.y) - toRealYY(point6.y)) / (toRealXX(point6.x) - toRealXX(point5.x))
            parent.b6 = toRealYY(point5.y) - parent.a6 * toRealXX(point5.x)

            parent.a7 =(toRealYY(point7.y) - toRealYY(point7.y)) / (toRealXX(point7.x) - toRealXX(point6.x))
            parent.b7 = toRealYY(point6.y) - parent.a7 * toRealXX(point6.x)

            parent.a8 =(toRealYY(point8.y) - toRealYY(point8.y)) / (toRealXX(point8.x) - toRealXX(point7.x))
            parent.b8 = toRealYY(point7.y) - parent.a8 * toRealXX(point7.x)

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
