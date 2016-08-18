import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import ChartModel 1.0
import GraphModel 1.0
import UIContextModel 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls
import "../../.." as Base

Rectangle {
    property GraphModel graph

    width : parent ? parent.width : 0
    x : 4
    function redraw() {
        chartpanel.chartData = {labels: chart.xvalues, datasets: chart.datasets(0)}
        chartpanel.update()
    }

    Column{
        id : propertiescol
        spacing : 6
        property int activeGraph : 0
        Row {
            Text{
                id : chartTypeLabel
                text : qsTr("Y Attribute")
                width : 70

            }

            ComboBox{
                id : charttype
                width : 150
                model : chart.graphs
                height : 16
                textRole: "yAxis"
                onCurrentIndexChanged: {
                    propertiescol.activeGraph = currentIndex
                    var g1 = chart.graph(currentIndex)
                    if ( g1){
                        if ( graph)
                            graph.enabled = false // removed old graph
                        graph = g1
                        graph.enabled = true
                        enabledcheck.checked = true
                        redraw()
                    }
                }
            }

        }
        CheckBox{
            id :enabledcheck
            text : qsTr("Enabled")
            checked : graph ? graph.enabled : false
            style : Base.CheckBoxStyle1{}
            onCheckedChanged: {
                if ( graph){
                    graph.enabled = checked
                    redraw()
                }
            }
        }
        Row {
            height : Global.rowHeight
            width : parent.width - 10
            spacing : 2
            Text {
                id : label6
                text : qsTr("Replace Undefined value")
                font.pointSize: 8
                width : 110
                wrapMode: Text.WordWrap
            }
            TextField{
                id : currentUndef
                width : 40
                height : 18
                text : "?"
            }
            TextField{
                id : newUndef
                width : 40
                height : 18
                text : ""
            }
            Button{
                text : "Apply"
                height : 18
                onClicked: {
                    graph.replaceUndefs(currentUndef.text, newUndef.text)
                    redraw()
                }
            }
        }
        GroupBox{
            id : colorgroup
            width : 266
            height : 109
            property bool initial : true
            title : qsTr("Colors")
            Column{
                width : parent.width
                Row {
                    height : Global.rowHeight
                    width : parent.width - 10
                    Text {
                        id : label2
                        text : qsTr("Fill")
                        width : 70
                    }
                    Controls.ColorPicker2{
                        id : fillcolor

                        onSelectedColorChanged: {
                            if ( graph && !colorgroup.initial){
                                graph.fillColor = selectedColor
                                redraw()
                            }
                        }
                    }
                    z : 4
                }
                Row {
                    height : Global.rowHeight
                    width : parent.width - 10
                    Text {
                        id : label3
                        text : qsTr("Stroke")
                        width : 70
                    }
                    Controls.ColorPicker2{
                        id : strokecolor
                        onSelectedColorChanged: {
                            if ( graph && !colorgroup.initial){
                                graph.strokeColor = selectedColor
                                redraw()
                            }
                        }
                    }
                    z : 3
                }
                Row {
                    height : Global.rowHeight
                    width : parent.width - 10
                    Text {
                        id : label4
                        text : qsTr("Point")
                        width : 70
                    }
                    Controls.ColorPicker2{
                        id : pointcolor
                        onSelectedColorChanged: {
                            if ( graph && !colorgroup.initial){
                                graph.pointColor = selectedColor
                                redraw()
                            }
                        }
                    }
                    z : 2
                }
                Row {
                    height : Global.rowHeight
                    width : parent.width - 10
                    Text {
                        id : label5
                        text : qsTr("Point Stroke")
                        width : 70
                    }
                    Controls.ColorPicker2{
                        id : pointstrokeColor
                        onSelectedColorChanged: {
                            if ( graph && !colorgroup.initial){
                                graph.pointStrokeColor = selectedColor
                                redraw()
                            }
                        }
                    }
                    z : 1
                }


            }
            Component.onCompleted: {
                if ( graph){
                    pointstrokeColor.selectedColor = graph.pointStrokeColor
                    pointcolor.selectedColor = graph.pointColor
                    strokecolor.selectedColor = graph.strokeColor
                    fillcolor.selectedColor = graph.fillColor
                }
                initial = false
            }
        }

    }
}

