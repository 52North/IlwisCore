import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

import "."
import "../../../qchart/QChart.js" as Charts
import "../../../qchart" as Graph


Rectangle {
    id :container

    color: "#ffffff";
    width : parent.width
    height : parent.height
    property alias chartData : chart_drawing.chartData
    property alias chartType : chart_drawing.chartType
    function update(){
        chart_drawing.requestPaint()
    }

    function updateChartWidth() {
        if ( chart)
            chart.updateChartWidth(chart_drawing.width)
    }

    onWidthChanged: {
        updateChartWidth()
    }

    Graph.Chart {
        id: chart_drawing;
        width: parent.width
        height: parent.height
        chartAnimated: true;
        chartAnimationEasing: Easing.InOutElastic;
        chartAnimationDuration: 1000;
    }
}

