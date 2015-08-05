import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "Global.js" as Global

CheckBoxStyle {
    indicator: Rectangle {
        implicitWidth: 14
        implicitHeight: 14
        radius: 2
        border.color: control.activeFocus ? "darkblue" : "gray"
        border.width: 1
        Rectangle {
            visible: control.checked
            color: "#3333A2"
            border.color: "#333"
            radius: 8
            anchors.margins: 3
            anchors.fill: parent
        }
    }
}

