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
        chartpanel.chartData = {labels: chart.xvalues, datasets: chart.datasets(1)}
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
        ReplaceUndefValues{}

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
            }
            Component.onCompleted: {
                if ( graph){
                    strokecolor.selectedColor = graph.strokeColor
                    fillcolor.selectedColor = graph.fillColor
                }
                initial = false
            }
        }

    }
}


