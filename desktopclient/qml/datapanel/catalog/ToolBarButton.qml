import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick 2.0

ToolButton{
    implicitHeight: heightButtons
    checkable: true
    checked: true
    style : ButtonStyle{
        background: Rectangle {
            anchors.fill: parent
            color : control.pressed || checked ? "#B0C4DE" : "white"
            border.width: 1
            border.color: "lightgrey"
        }
    }
}
