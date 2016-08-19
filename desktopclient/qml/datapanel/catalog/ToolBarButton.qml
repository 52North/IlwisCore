import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick 2.0
import "../../controls" as Controls

ToolButton{
    id : but
    implicitHeight: heightButtons
    checkable: true
    checked: true
    opacity: checked ? 1 : 0.35
    property string tooltipText

    style : ButtonStyle{
        background: Rectangle {

            anchors.fill: parent
            color : control.pressed || checked ? "#B0C4DE" : "white"
            border.width: 1
            border.color: "lightgrey"


        }
    }
    Controls.ToolTip{
        target : but
        text : tooltipText
    }
}
