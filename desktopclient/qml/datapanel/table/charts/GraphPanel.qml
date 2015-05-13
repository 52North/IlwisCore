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
    //property variant xvalues :  ["January","February","March","April","May","June","July"]
   // property var datasets : [{fillColor : "transparent",strokeColor :"#009092",pointColor : "DarkBlue",pointStrokeColor :"DarkSlateBlue",data : [65,59,90,81,56,55,40]}]

    property var xvalues :  chart ? chart.xvalues : null
    property var datasets : chart ? chart.datasets : null



    Graph.Chart {
        id: chart_line;
        width: parent.width
        height: parent.height
        chartAnimated: true;
        chartAnimationEasing: Easing.InOutElastic;
        chartAnimationDuration: 1000;
        chartType: Charts.ChartType.LINE;

        Component.onCompleted: {
            chartData = { labels: container.xvalues,
                datasets: container.datasets
            }
        }
    }
}

