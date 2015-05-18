import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import ChartModel 1.0
import GraphModel 1.0
import UIContextModel 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Rectangle {
    property GraphModel graph

    width : parent.width
    x : 4
    function redraw() {
        chartpanel.chartData = {labels: chart.xvalues, datasets: chart.datasets}
        chartpanel.update()
    }

    Column{
        spacing : 3
        //        CheckBox{
        //            text : qsTr("Selection only")
        //        }

        Row {
            Text{
                id : chartTypeLabel
                text : qsTr("Y Attribute")
                width : 70

            }

            ComboBox{
                id : charttype
                width : 150
                model : chart.yAttributes
                height : 16
                onCurrentIndexChanged: {
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
                    Controls.ColorPicker{
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
                    Controls.ColorPicker{
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
                    Controls.ColorPicker{
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
                    Controls.ColorPicker{
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
                initial = false
            }
        }

    }
}

