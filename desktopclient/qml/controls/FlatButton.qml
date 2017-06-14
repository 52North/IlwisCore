import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import QtQuick.Window 2.1
import "../Global.js" as Global

Button {
    property string icon
    property alias label : buttxt.text
    property alias tooltip : ttip.text

    y : 2
    width : 62
    height : parent.height - 4
    clip : true
    Image {
        id : plaatje
        source : icon
        width : 30
        height : 28
        y : 2
        anchors.horizontalCenter: parent.horizontalCenter
    }
    Text {
        id : buttxt
        anchors.top : plaatje.bottom
        text : label
        color : "white"
        elide: Text.ElideMiddle
        anchors.horizontalCenter: parent.horizontalCenter
    }
    ToolTip{
        id : ttip
        target : parent
    }

    style : ButtonStyle{
        background: Rectangle {
            anchors.fill: parent
            color : control.pressed ? uicontext.paleColor : uicontext.darkColor
        }
    }
}
