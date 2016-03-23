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
        chartpanel.chartData =chart.datasets(3)
        chartpanel.update()
    }


    Rectangle{
        id : items
        width : parent.width - 10
        height : 200
        color : Global.alternatecolor3
        ScrollView {
            anchors.fill: parent
            ListView {

                anchors.fill: parent
                model : chart.xvalues
                clip :true
                delegate : Item{
                    width: 160
                    height : 18
                    Rectangle{
                        id : colorrect
                        width : 14
                        height : 14
                        y : 3
                        color : graph.ycolor(index)
                    }

                    Text{
                        height : 18
                        width : 140
                        anchors.top : colorrect.top
                        anchors.left : colorrect.right
                        anchors.leftMargin: 3
                        text : modelData
                        MouseArea{
                            anchors.fill : parent
                            onClicked: {
                                colorpicker.enabled = true;
                                colorpicker.opacity = 1
                            }
                        }
                    }

                }


            }
        }
    }

    Row {
        id : colorpicker
        height : Global.rowHeight
        width : parent.width - 10
        anchors.top : items.bottom
        anchors.topMargin: 4
        enabled: false
        opacity : 0
        Text {
            id : label2
            text : qsTr("Fill")
            width : 70
        }
        Controls.ColorPicker2{
            id : fillcolor

            onSelectedColorChanged: {
                if ( graph ){
                    graph.fillColor = selectedColor
                    redraw()
                }
            }
        }
        z : 4

    }

}







