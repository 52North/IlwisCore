import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import ChartModel 1.0
import GraphModel 1.0
import UIContextModel 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls
import "../../../qchart/QChart.js" as Charts

Item {
    width : 270
    height : parent.height - 10

    function init(){
        makechart(table,0,0)
        var graph = chart.graph(0) // by default we choose the first one
        specificProperties.setSource("LineChartProperties.qml",{ "graph": graph })
    }

    function makechart(table, xchoice, typechoice){
        chart.setXAxis(xchoice)
        chart.setGraphs(typechoice)
        if ( typechoice === 0){
            chartpanel.chartType = Charts.ChartType.LINE
            chartpanel.chartData = {datasets: chart.datasets(typechoice),labels: chart.xvalues}
        }else if ( typechoice === 1){
            chartpanel.chartType = Charts.ChartType.BAR
            chartpanel.chartData = {labels: chart.xvalues, datasets: chart.datasets(typechoice)}
        }else if ( typechoice === 2){
            chartpanel.chartType = Charts.ChartType.PIE
            chartpanel.chartData = chart.datasets(typechoice)
        }else if ( typechoice === 3){
            chartpanel.chartType = Charts.ChartType.POLAR
            chartpanel.chartData = chart.datasets(typechoice)
        }

        chartpanel.update()
    }

    Column{
        id : ychoices
        x : 4
        spacing : 3
        width : parent.width
        height : childrenRect.height

        Row{
            Text{
                text : qsTr("X axis")
                width : 70

            }

            ComboBox{
                id : xaxis
                model : table ? table.columns : null
                width : 150
                textRole: "attributename"
                height : 16
                onCurrentIndexChanged: {
                    if ( chart){
                        makechart(table,xaxis.currentIndex, charttype.currentIndex)
                        var graph = chart.graph(0) // by default we choose the first one
                        specificProperties.sourceComponent = null
                        specificProperties.setSource("LineChartProperties.qml",{ "graph": graph })
                    }
                }
            }
        }
        Row{
            Text{
                text : qsTr("Chart type")
                width : 70

            }

            ComboBox{
                id : charttype
                width : 150
                model : ["Line", "Bar","Pie","Polar","Radar","Doughnut"]
                height : 16
                onCurrentIndexChanged: {
                    if (chart){
                        makechart(table, xaxis.currentIndex,charttype.currentIndex)
                        var graph = chart.graph(0) // by default we choose the first one
                        if ( graph){
                            if ( currentIndex == 0){
                                specificProperties.setSource("LineChartProperties.qml",{ "graph": graph })
                            } else if (currentIndex == 1){
                                specificProperties.setSource("BarChartProperties.qml",{ "graph": graph })
                            } else if (currentIndex == 2){
                                specificProperties.setSource("PieChartProperties.qml",{ "graph": graph })
                            } else if (currentIndex == 3){
                                specificProperties.setSource("PolarChartProperties.qml",{ "graph": graph })
                            }
                        }
                    }
                }
            }
        }

    }
    Loader {
        width : parent.width
        height : parent.height - ychoices.height - 4
        anchors.top: ychoices.bottom
        anchors.topMargin: 4
        anchors.leftMargin: 4
        id : specificProperties

    }



}

