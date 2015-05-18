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
    property alias chartData : chart_line.chartData
    function update(){
        chart_line.requestPaint()
    }

    Graph.Chart {
        id: chart_line;
        width: parent.width
        height: parent.height
        chartAnimated: true;
        chartAnimationEasing: Easing.InOutElastic;
        chartAnimationDuration: 1000;
        chartType: Charts.ChartType.LINE;

        Component.onCompleted: {

        }
    }
}

