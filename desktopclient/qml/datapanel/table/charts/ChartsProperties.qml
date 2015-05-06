import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import ChartModel 1.0
import GraphModel 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Item {
    width : 240
    height : parent.height - 10

    Column{
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
                model : table.columns
                width : 150
                textRole: "attributename"
                height : 16
            }
        }
        Row{
            Text{
                text : qsTr("Chart type")
                width : 70

            }

            ComboBox{
                width : 150
                model : ["Line", "Bar","Pie","Polar","Radar","Doughnut"]
                height : 16
            }
        }
    }

}

