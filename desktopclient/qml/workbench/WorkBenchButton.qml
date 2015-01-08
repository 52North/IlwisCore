import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../controls" as Controls

ToolButton{
    property string iconname
    id : nav
    height : parent.width// buttonB.width
    width : parent.width
    Image { anchors.centerIn: parent;  source: "../images/" + iconname; fillMode: Image.PreserveAspectFit }
    style : ButtonStyle{
        background: Rectangle {
            anchors.fill: parent
            color : control.pressed ? "#B0C4DE" : "white"
            border.width: 1
            border.color: "lightgrey"
        }
    }
}
