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

    property int minX: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.min : 0
    property int maxX: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.max : 0
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
                selectedNode.standardization.standardizationValue.setAnchor(x, y)
                graphCanvas.repaint()
            }
        }

        Repeater {
            model: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors : null
            delegate: DragPoint {
                x: toViewportXX(model.x)
                y: toViewportYY(model.y)
            }
        }

        function drawFunction(ctx) {
            if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) {
                ctx.strokeStyle = "blue"
                ctx.beginPath()
                var iX = graphCanvas.axisOffset
                var rX = toRealXX(iX)
                var rY = selectedNode.standardization.standardizationValue.getFx(rX)
                var iY = toViewportYY(rY)
                ctx.moveTo(0.5 + iX, 0.5 + iY)
                for (iX = graphCanvas.axisOffset + 1; iX <= graphCanvas.width - graphCanvas.axisOffset; ++iX)
                {
                    rX = toRealXX(iX)
                    rY = selectedNode.standardization.standardizationValue.getFx(rX)
                    iY = toViewportYY(rY)
                    ctx.lineTo(0.5 + iX, 0.5 + iY)
                }
                ctx.lineTo(0.5 + iX, 0.5 + iY) // include the last point in the plot (it is not included in the loop's lineTo)
                ctx.stroke() // actually draw the path that was just defined
            }
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
            drawFunction(ctx)
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

    Repeater {
        id: editor
        model: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.anchors : null
        delegate {
            Row {
                Controls.TextEditLabelPair {
                    width: 100
                    height: 20
                    labelWidth: 20
                    content: model.x.toFixed(3).toString()
                    regexvalidator: /^-?\d*(\.\d*)?$/
                    labelText: ("x" + index)
                    readOnly: (index == 0) || (index == editor.count - 1) // make first and last X read-only (piecewise linear)
                    onContentEdited: {
                        model.x = parseFloat(content);
                        selectedNode.standardization.standardizationValue.solveParams()
                        graphCanvas.repaint()
                    }
                }
                Controls.TextEditLabelPair {
                    width: 80
                    height: 20
                    labelWidth: 20
                    content: model.y.toFixed(3).toString()
                    regexvalidator: /^-?\d*(\.\d*)?$/
                    labelText: ("y" + index)
                    onContentEdited: {
                        model.y = parseFloat(content)
                        selectedNode.standardization.standardizationValue.solveParams()
                        graphCanvas.repaint()
                    }
                }
            }
        }
    }
}
