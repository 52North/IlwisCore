import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Rectangle {
    width: 300
    height: parent ? parent.height - 10 : 0
    property var editor
    x : 5

    Text{
        id : label
        text : qsTr("Background color")
    }

    Controls.ColorPicker2{

        onSelectedColorChanged: {
            renderer.addCommand("setbackgroundcolor(" + renderer.viewerId + "," + selectedColor + ")")
            renderer.update()
        }

        anchors.top : label.bottom
        anchors.topMargin: 4
//        lefttopColor: "white"
//        leftbottomColor: "Navy"
//        righttopColor: "black"
//        rightbottomColor: "#FFEFD5"
    }
}

