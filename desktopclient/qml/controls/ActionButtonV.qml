import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global

Button{
    property string iconsource
    property string buttontext;

    id : addContainer
//    width : 40
//    height : 80
    Image {
        id : buttonIcon
        anchors.horizontalCenter: parent.horizontalCenter
        source: iconsource
        y : 5 ;width : 25* Global.uiScale; height : 25* Global.uiScale
        fillMode: Image.PreserveAspectFit
    }
    Text {
        anchors.top: buttonIcon.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text : buttontext
        font.pointSize: 9 * Global.uiScale
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
