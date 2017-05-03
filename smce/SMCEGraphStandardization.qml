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

    property double minX: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.min : 0
    property double maxX: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? selectedNode.standardization.standardizationValue.max : 0
    property double minY: 0
    property double maxY: 1

    function refresh() {
        graphCanvas.repaint()
    }

    Row {
        GroupBox {
            id: costbenefit
            ColumnLayout {
                ExclusiveGroup { id: costbenefitGroup }
                RadioButton {
                    id: modeBenefit
                    text: qsTr("Benefit")
                    checked: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? (selectedNode.standardization.standardizationValue.benefit && selectedNode.standardization.standardizationValue.method <= 5) : false
                    exclusiveGroup: costbenefitGroup
                    property bool benefit: true
                    onClicked: {
                        if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null)
                            selectedNode.standardization.standardizationValue.setMethod(methodCostBenefitGroup.current.method, costbenefitGroup.current.benefit)
                        graphCanvas.repaint()
                    }
                }
                RadioButton {
                    id: modeCost
                    text: qsTr("Cost")
                    checked: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? (!selectedNode.standardization.standardizationValue.benefit && selectedNode.standardization.standardizationValue.method <= 5) : false
                    exclusiveGroup: costbenefitGroup
                    property bool benefit: false
                    onClicked: {
                        if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null)
                            selectedNode.standardization.standardizationValue.setMethod(methodCostBenefitGroup.current.method, costbenefitGroup.current.benefit)
                        graphCanvas.repaint()
                    }
                }
                RadioButton {
                    id: modeCombination
                    text: qsTr("Combination")
                    checked: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? (selectedNode.standardization.standardizationValue.method > 5) : false
                    exclusiveGroup: costbenefitGroup
                    property bool benefit: true
                    onClicked: {
                        if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) {
                            if (methodCombinationGroup.current)
                                selectedNode.standardization.standardizationValue.setMethod(methodCombinationGroup.current.method, methodCombinationGroup.current.benefit)
                            else
                                selectedNode.standardization.standardizationValue.setMethod(6, true)
                        }
                        graphCanvas.repaint()
                    }
                }
            }
        }

        GroupBox {
            id: methodCostBenefit
            visible: modeBenefit.checked || modeCost.checked
            enabled: modeBenefit.checked || modeCost.checked
            ColumnLayout {
                ExclusiveGroup { id: methodCostBenefitGroup }
                RadioButton {
                    id: methodMaximum
                    text: qsTr("Maximum")
                    checked: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? (selectedNode.standardization.standardizationValue.method === 1) : false
                    exclusiveGroup: methodCostBenefitGroup
                    property int method : 1
                    onClicked: {
                        if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null)
                            selectedNode.standardization.standardizationValue.setMethod(methodCostBenefitGroup.current.method, costbenefitGroup.current.benefit)
                        graphCanvas.repaint()
                    }
                }
                RadioButton {
                    id: methodInterval
                    text: qsTr("Interval")
                    checked: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? (selectedNode.standardization.standardizationValue.method === 2) : false
                    exclusiveGroup: methodCostBenefitGroup
                    property int method : 2
                    onClicked: {
                        if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null)
                            selectedNode.standardization.standardizationValue.setMethod(methodCostBenefitGroup.current.method, costbenefitGroup.current.benefit)
                        graphCanvas.repaint()
                    }
                }
                RadioButton {
                    id: methodGoal
                    text: qsTr("Goal")
                    checked: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? (selectedNode.standardization.standardizationValue.method === 3) : false
                    exclusiveGroup: methodCostBenefitGroup
                    property int method : 3
                    onClicked: {
                        if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null)
                            selectedNode.standardization.standardizationValue.setMethod(methodCostBenefitGroup.current.method, costbenefitGroup.current.benefit)
                        graphCanvas.repaint()
                    }
                }
                RadioButton {
                    id: methodConvex
                    text: qsTr("Convex")
                    checked: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? (selectedNode.standardization.standardizationValue.method === 4) : false
                    exclusiveGroup: methodCostBenefitGroup
                    property int method : 4
                    onClicked: {
                        if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null)
                            selectedNode.standardization.standardizationValue.setMethod(methodCostBenefitGroup.current.method, costbenefitGroup.current.benefit)
                        graphCanvas.repaint()
                    }
                }
                RadioButton {
                    id: methodConcave
                    text: qsTr("Concave")
                    checked: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? (selectedNode.standardization.standardizationValue.method === 5) : false
                    exclusiveGroup: methodCostBenefitGroup
                    property int method : 5
                    onClicked: {
                        if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null)
                            selectedNode.standardization.standardizationValue.setMethod(methodCostBenefitGroup.current.method, costbenefitGroup.current.benefit)
                        graphCanvas.repaint()
                    }
                }
            }
        }

        GroupBox {
            id: methodCombination
            visible: modeCombination.checked
            enabled: modeCombination.checked
            ColumnLayout {
                ExclusiveGroup { id: methodCombinationGroup }
                RadioButton {
                    id: methodUShapeUp
                    text: qsTr("U-Shape, up")
                    checked: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? (selectedNode.standardization.standardizationValue.benefit && selectedNode.standardization.standardizationValue.method === 6) : false
                    exclusiveGroup: methodCombinationGroup
                    property int method : 6
                    property bool benefit : true
                    onClicked: {
                        if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null)
                            selectedNode.standardization.standardizationValue.setMethod(methodCombinationGroup.current.method, methodCombinationGroup.current.benefit)
                        graphCanvas.repaint()
                    }
                }
                RadioButton {
                    id: methodUShapeDown
                    text: qsTr("U-Shape, down")
                    checked: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? (!selectedNode.standardization.standardizationValue.benefit && selectedNode.standardization.standardizationValue.method === 6) : false
                    exclusiveGroup: methodCombinationGroup
                    property int method : 6
                    property bool benefit : false
                    onClicked: {
                        if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null)
                            selectedNode.standardization.standardizationValue.setMethod(methodCombinationGroup.current.method, methodCombinationGroup.current.benefit)
                        graphCanvas.repaint()
                    }
                }
                RadioButton {
                    id: methodGaussianUp
                    text: qsTr("Gaussian, up")
                    checked: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? (selectedNode.standardization.standardizationValue.benefit && selectedNode.standardization.standardizationValue.method === 7) : false
                    exclusiveGroup: methodCombinationGroup
                    property int method : 7
                    property bool benefit : true
                    onClicked: {
                        if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null)
                            selectedNode.standardization.standardizationValue.setMethod(methodCombinationGroup.current.method, methodCombinationGroup.current.benefit)
                        graphCanvas.repaint()
                    }
                }
                RadioButton {
                    id: methodGaussianDown
                    text: qsTr("Gaussian, down")
                    checked: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? (!selectedNode.standardization.standardizationValue.benefit && selectedNode.standardization.standardizationValue.method === 7) : false
                    exclusiveGroup: methodCombinationGroup
                    property int method : 7
                    property bool benefit : false
                    onClicked: {
                        if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null)
                            selectedNode.standardization.standardizationValue.setMethod(methodCombinationGroup.current.method, methodCombinationGroup.current.benefit)
                        graphCanvas.repaint()
                    }
                }
                RadioButton {
                    id: methodPiecewiseLinear
                    text: qsTr("Piecewise Linear")
                    checked: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null) ? (selectedNode.standardization.standardizationValue.method === 8) : false
                    exclusiveGroup: methodCombinationGroup
                    property int method : 8
                    property bool benefit : true
                    onClicked: {
                        if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValue !== null)
                            selectedNode.standardization.standardizationValue.setMethod(methodCombinationGroup.current.method, methodCombinationGroup.current.benefit)
                        graphCanvas.repaint()
                    }
                }
            }
        }
    }

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
