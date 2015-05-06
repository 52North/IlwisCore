import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "./charts" as Charts
import ChartModel 1.0
import GraphModel 1.0

SplitView {
    anchors.fill : parent
    anchors.topMargin: 4
    Charts.ChartsProperties {
        width : 240
        height : parent.height
    }
    Charts.GraphPanel{
        height : parent.height
        Layout.fillWidth: true
    }

}

