import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "./charts" as Charts
import ChartModel 1.0
import GraphModel 1.0

SplitView {
    id : chartspane
    anchors.fill : parent
    anchors.topMargin: 4
    objectName:  uicontext.uniqueName()
    property ChartModel chart

    onChartChanged: {
         chartpanel.chartData = {labels: chart.xvalues, datasets: chart.datasets}
    }

    Charts.ChartsProperties {
        id : propertiespanel
        width : 270
        height : parent.height
    }
    Charts.GraphPanel{
        id : chartpanel
        height : parent.height
        Layout.fillWidth: true
    }
    Component.onCompleted: {
        chart = uicontext.chartModel(objectName,table)
        propertiespanel.makechart(table,0,0)
    }

}

