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
    Column{

        CheckBox{
            text : qsTr("Selection only")
        }
        Text{
            text : qsTr("Chart type")
            width : 70

        }

        ComboBox{
            id : charttype
            width : 150
            model : chart.yattributes
            height : 16
            onCurrentIndexChanged: {
            }
        }

    }

}

