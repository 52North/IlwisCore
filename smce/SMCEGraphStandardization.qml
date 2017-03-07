import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../qml/controls" as Controls
import "../../../qml/Global.js" as Global


Column {
    id : createForm
    width : stdEditor.width
    spacing: 1

    signal executeStandardization()

    property int minX: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.min : 0
    property int maxX: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.max : 0
    property int minY: 0
    property int maxY: 1

    Canvas {
        id: graphCanvas
        width: parent.width
        height: 200
        property int axisOffset : 20

        MouseArea {
            anchors.fill: parent
            onPositionChanged: {
                var x = toRealXX(mouse.x)
                var y = toRealYY(mouse.y)
                selectedNode.standardization.standardizationValue.SetAnchor(x, y)
                graphCanvas.repaint()
            }
        }

        // Repeaters and ListViews are too slow for drag functionality,
        // so we create the dragable points statically
        DragPoint {
            id: point0
            x: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportXX(selectedNode.standardization.standardizationValue.anchors[0].x) : 0
            y: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportYY(selectedNode.standardization.standardizationValue.anchors[0].y) : 0
        }

        DragPoint {
            id: point1
            x: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportXX(selectedNode.standardization.standardizationValue.anchors[1].x) : 0
            y: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportYY(selectedNode.standardization.standardizationValue.anchors[1].y) : 0
        }

        DragPoint {
            id: point2
            x: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportXX(selectedNode.standardization.standardizationValue.anchors[2].x) : 0
            y: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportYY(selectedNode.standardization.standardizationValue.anchors[2].y) : 0
        }

        DragPoint {
            id: point3
            x: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportXX(selectedNode.standardization.standardizationValue.anchors[3].x) : 0
            y: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportYY(selectedNode.standardization.standardizationValue.anchors[3].y) : 0
        }

        DragPoint {
            id: point4
            x: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportXX(selectedNode.standardization.standardizationValue.anchors[4].x) : 0
            y: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportYY(selectedNode.standardization.standardizationValue.anchors[4].y) : 0
        }

        DragPoint {
            id: point5
            x: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportXX(selectedNode.standardization.standardizationValue.anchors[5].x) : 0
            y: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportYY(selectedNode.standardization.standardizationValue.anchors[5].y) : 0
        }

        DragPoint {
            id: point6
            x: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportXX(selectedNode.standardization.standardizationValue.anchors[6].x) : 0
            y: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportYY(selectedNode.standardization.standardizationValue.anchors[6].y) : 0
        }

        DragPoint {
            id: point7
            x: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportXX(selectedNode.standardization.standardizationValue.anchors[7].x) : 0
            y: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportYY(selectedNode.standardization.standardizationValue.anchors[7].y) : 0
        }

        DragPoint {
            id: point8
            x: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportXX(selectedNode.standardization.standardizationValue.anchors[8].x) : 0
            y: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? toViewportYY(selectedNode.standardization.standardizationValue.anchors[8].y) : 0
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
            ctx.stroke() // actually draw the path that was just defined
        }

        function drawAxis(ctx) {
            ctx.lineWidth = 1.0
            ctx.strokeStyle = "black"
            ctx.beginPath()
            ctx.moveTo(0.5 + graphCanvas.axisOffset, 0.5 + graphCanvas.axisOffset) // top-left
            ctx.lineTo(0.5 + graphCanvas.axisOffset, 0.5 + graphCanvas.height - graphCanvas.axisOffset) // origin
            ctx.lineTo(0.5 + graphCanvas.width - graphCanvas.axisOffset, 0.5 + graphCanvas.height - graphCanvas.axisOffset) // bottom-right

            // draw ticks on the axes
            var nrTicks = 5
            var tickThickness = 3 // nr of pixels for drawing a tick
            var tickXSize = (graphCanvas.width - 2 * graphCanvas.axisOffset) / (nrTicks - 1)
            var tickYSize = (graphCanvas.height - 2 * graphCanvas.axisOffset) / (nrTicks - 1)
            for (var i = 0; i < nrTicks; ++i) {
                ctx.moveTo(0.5 + graphCanvas.axisOffset, 0.5 + graphCanvas.axisOffset + Math.round(tickYSize * i))
                ctx.lineTo(0.5 + graphCanvas.axisOffset - tickThickness, 0.5 + graphCanvas.axisOffset + Math.round(tickYSize * i))
                ctx.moveTo(0.5 + graphCanvas.axisOffset + Math.round(tickXSize * i), 0.5 + graphCanvas.height - graphCanvas.axisOffset)
                ctx.lineTo(0.5 + graphCanvas.axisOffset + Math.round(tickXSize * i), 0.5 + graphCanvas.height - graphCanvas.axisOffset + tickThickness)
            }
            ctx.stroke() // actually draw the path that was just defined

            // x-axis values
            ctx.fillStyle = "#000";
            ctx.textAlign = "center"
            ctx.textBaseline="top"
            ctx.fillText(minX.toString(), graphCanvas.axisOffset, graphCanvas.height - graphCanvas.axisOffset + tickThickness)
            ctx.fillText(maxX.toString(), graphCanvas.width - graphCanvas.axisOffset, graphCanvas.height - graphCanvas.axisOffset + tickThickness)
            // unit
            ctx.fillText((selectedNode != null && selectedNode.unit != "") ? ("[" + selectedNode.unit + "]") : "", graphCanvas.width / 2, graphCanvas.height - graphCanvas.axisOffset + tickThickness)
            // y-axis values
            ctx.textAlign = "right"
            ctx.textBaseline="middle"
            ctx.fillText(maxY.toString(), graphCanvas.axisOffset - tickThickness, graphCanvas.axisOffset)
            ctx.fillText(minY.toString(), graphCanvas.axisOffset - tickThickness, graphCanvas.height - graphCanvas.axisOffset)
        }

        onPaint : {
            var ctx = getContext("2d")
            ctx.fillStyle = "white"
            ctx.fillRect(0, 0, graphCanvas.width, graphCanvas.height)
            drawAxis(ctx)
            initializeCurve(ctx)
        }

        function repaint() {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, graphCanvas.width, graphCanvas.height)
            requestPaint()
        }
    }

    function toRealXX(x) {
        var scalerange = maxX - minX
        var graphXXrange = graphCanvas.width - 2 * graphCanvas.axisOffset
        return minX + (scalerange/graphXXrange) * (x - graphCanvas.axisOffset) // axisOffset = XX origin
    }

    function toRealYY(y) {
        var scalerange = maxY - minY
        var graphYYrange = graphCanvas.height - 2 * graphCanvas.axisOffset
        return maxY - (scalerange/graphYYrange) * (y - graphCanvas.axisOffset)
    }

    function toViewportXX(x) {
        var scalerange = maxX - minX
        var graphXXrange = graphCanvas.width - 2 * graphCanvas.axisOffset
        return Math.round(graphCanvas.axisOffset + (x - minX) * (graphXXrange/scalerange)) // axisOffset = XX origin
    }

    function toViewportYY(y) {
        var scalerange = maxY - minY
        var graphYYrange = graphCanvas.height - 2 * graphCanvas.axisOffset
        return Math.round(graphCanvas.height - graphCanvas.axisOffset - (y - minY) * (graphYYrange/scalerange))
    }

    Row {
        Controls.TextEditLabelPair {
            id: objCoordX0text
            width: 100
            height: 20
            labelWidth: 20
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[0].x.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x0")
            readOnly: true
        }
        Controls.TextEditLabelPair {
            id: objCoordY0text
            width: 80
            height: 20
            labelWidth: 20
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[0].y.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y0")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[0].y = parseFloat(content)
                selectedNode.standardization.standardizationValue.SolveParams()
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
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[1].x.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x1")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[1].x = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
                graphCanvas.repaint()
            }
        }
        Controls.TextEditLabelPair {
            id: objCoordY1text
            width: 80
            height: 20
            labelWidth: 20
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[1].y.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y1")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[1].y = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
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
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[2].x.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x2")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[2].x = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
                graphCanvas.repaint()
            }
        }
        Controls.TextEditLabelPair {
            id: objCoordY2text
            width: 80
            height: 20
            labelWidth: 20
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[2].y.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y2")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[2].y = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
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
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[3].x.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x3")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[3].x = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
                graphCanvas.repaint()
            }
        }
        Controls.TextEditLabelPair {
            id: objCoordY3text
            width: 80
            height: 20
            labelWidth: 20
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[3].y.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y3")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[3].y = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
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
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[4].x.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x4")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[4].x = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
                graphCanvas.repaint()
            }
        }
        Controls.TextEditLabelPair {
            id: objCoordY4text
            width: 80
            height: 20
            labelWidth: 20
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[4].y.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y4")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[4].y = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
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
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[5].x.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x5")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[5].x = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
                graphCanvas.repaint()
            }
        }
        Controls.TextEditLabelPair {
            id: objCoordY5text
            width: 80
            height: 20
            labelWidth: 20
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[5].y.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y5")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[5].y = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
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
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[6].x.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x6")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[6].x = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
                graphCanvas.repaint()
            }
        }
        Controls.TextEditLabelPair {
            id: objCoordY6text
            width: 80
            height: 20
            labelWidth: 20
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[6].y.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y6")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[6].y = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
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
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[7].x.toFixed(3).toString() : ""
            //color:"black"
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x7")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[7].x = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
                graphCanvas.repaint()
            }
        }
        Controls.TextEditLabelPair {
            id: objCoordY7text
            width: 80
            height: 20
            labelWidth: 20
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[7].y.toFixed(3).toString() : ""
            //color:"black"
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y7")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[7].y = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
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
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[8].x.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("x8")
            readOnly: true
        }
        Controls.TextEditLabelPair {
            id: objCoordY8text
            width: 80
            height: 20
            labelWidth: 20
            content: (selectedNode !== null && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors[8].y.toFixed(3).toString() : ""
            regexvalidator: /^-?\d*(\.\d*)?$/
            labelText: qsTr("y8")
            onContentEdited: {
                selectedNode.standardization.standardizationValue.anchors[8].y = parseFloat(content);
                selectedNode.standardization.standardizationValue.SolveParams()
                graphCanvas.repaint()
            }
        }
    }
}
