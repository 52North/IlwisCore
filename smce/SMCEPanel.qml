import QtQuick 2.0
import SMCE 1.0

import "../../../qml/controls" as Controls

Column {
    anchors.fill: parent
    SMCEToolBar{
        id : tools
    }
    Rectangle {
        width : parent.width
        height : parent.height - tools.height
        color : "#FFF8DC"
        Text{
            anchors.centerIn: parent
            text: "I am under construction"
            font.bold: true
            font.pointSize: 20

        }
    }
}
