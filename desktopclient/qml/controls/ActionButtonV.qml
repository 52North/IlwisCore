import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global

Button{
    property string iconsource
    property string buttontext;
    property string backColor : "white"

    id : addContainer
    Image {
        id : buttonIcon
        anchors.horizontalCenter: parent.horizontalCenter
        source: iconsource
    }
    Text {
        anchors.bottom: addContainer.bottom
        anchors.bottomMargin: 2
        anchors.horizontalCenter: parent.horizontalCenter
        text : buttontext
        font.pointSize: 9
    }

    style : ButtonStyle{
        background: Rectangle {
            anchors.fill: parent
            color : control.pressed ? uicontext.paleColor : backColor
        }
    }

    onClicked: {
        if ( typeof addContainer.mouseClicked === "function"){
            checked = !checked
            mouseClicked()
        }
    }

}
