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

Row {
    height : Global.rowHeight
    width : parent.width - 10
    spacing : 2
    Text {
        id : label6
        text : qsTr("Replace Undefined value")
        font.pointSize: 8
        width : 110
        wrapMode: Text.WordWrap
    }
    TextField{
        id : currentUndef
        width : 40
        height : 18
        text : "?"
    }
    TextField{
        id : newUndef
        width : 40
        height : 18
        text : ""
    }
    Button{
        text : "Apply"
        height : 18
        onClicked: {
            graph.replaceUndefs(currentUndef.text, newUndef.text)
            redraw()
        }
    }
}

