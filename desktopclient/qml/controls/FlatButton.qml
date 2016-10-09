import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import QtQuick.Window 2.1
import "../Global.js" as Global

Button {
    property string icon
    property string label

    y : 2
    width : 62
    height : parent.height - 4
    Image {
        id : plaatje
        source : icon
        width : parent.width - 32
        height : 28
        y : 2
        anchors.horizontalCenter: parent.horizontalCenter
    }
    Text {
        anchors.top : plaatje.bottom
        text : label
        color : "white"
        anchors.horizontalCenter: parent.horizontalCenter
    }

    style : ButtonStyle{
        background: Rectangle {
            anchors.fill: parent
            color : control.pressed ? Global.palegreen : Global.darkgreen
        }
    }
}
