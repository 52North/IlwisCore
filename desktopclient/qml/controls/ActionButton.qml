import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1

Button{
    property string iconsource
    property string buttontext;

    id : addContainer
    width : 40
    height : 40
    Image {
        id : buttonIcon
        anchors.verticalCenter:  parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
        source: iconsource
        y : 5 ;width : 30; height : 30
        fillMode: Image.PreserveAspectFit
    }
    Text {
        anchors.left: buttonIcon.right
        anchors.leftMargin: 3
        anchors.verticalCenter:  parent.verticalCenter
        text : buttontext
    }

    style : ButtonStyle{
        background: Rectangle {
            anchors.fill: parent
            color : control.pressed || (checked) ? "#B0C4DE" : "white"
            border.width: 1
            border.color: "lightgrey"
        }
    }

}
