import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import ChartModel 1.0
import GraphModel 1.0
import UIContextModel 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Item {
    width : 270
    height : parent.height - 10

    function makechart(table, xchoice, typechoice){
        chart.setXAxis(xchoice)
        var graph = chart.setGraphs(typechoice)
        chartpanel.chartData = {labels: chart.xvalues, datasets: chart.datasets}
        chartpanel.update()
        return graph
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
                model : table.columns
                width : 150
                textRole: "attributename"
                height : 16
                onCurrentIndexChanged: {
                    if ( chart){
                           makechart(table,xaxis.currentIndex, charttype.currentIndex)
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
                    if ( chart && currentIndex == 0){
                        var graph = makechart(table, xaxis.currentIndex,charttype.currentIndex)
                        specificProperties.setSource("LineChartProperties.qml",{ "graph": graph })
                    }
                }
            }
        }

    }
    Loader {
        anchors.top: ychoices.bottom
        anchors.topMargin: 4
        anchors.leftMargin: 4
        id : specificProperties

    }



}

