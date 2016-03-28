import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1

Button{
    property string iconsource
    property string buttontext;

    id : addContainer
    Image {
        id : buttonIcon
        anchors.verticalCenter: parent.verticalCenter
        source: iconsource
        x : 5 ;width : 25; height : 25
        fillMode: Image.PreserveAspectFit
    }
    Text {
        anchors.left: buttonIcon.right
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
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


