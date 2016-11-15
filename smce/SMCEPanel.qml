import QtQuick 2.0
import SMCE 1.0

import "../../../qml/controls" as Controls

Column {
    id : column
    anchors.fill: parent
    property alias toolbar : tools

    SMCEToolBar{
        id : tools
    }
    Rectangle {
        id : smcearea
        width : parent.width
        height : parent.height - tools.height
        color : "#FFF8DC"
        opacity : height > 0 ? 1 : 0
        Text{
            anchors.centerIn: parent
            text: "SMCE Defenition and Evaluation Construction"
            font.bold: true
            font.pointSize: 20


        }

    }
}
