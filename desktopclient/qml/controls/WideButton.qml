import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global

Button {
    id : b1
    property string image
    property string label
    Image {
        id : icon
        x : 4 + 4
        height : 20
        source : image
        anchors.verticalCenter: parent.verticalCenter
    }
    Text {
        anchors.left : icon.right
        anchors.leftMargin: 6
        text : label
        font.pointSize: 8
        anchors.verticalCenter: parent.verticalCenter
    }


    style: ButtonStyle {
        background: Rectangle {
            implicitWidth: control.width
            implicitHeight: control.height
            border.color: Global.edgecolor
            border.width: control.pressed ? 2 : 1
            radius: 2
            color : control.pressed ? Global.selectedColor : "white"
        }
    }
}

