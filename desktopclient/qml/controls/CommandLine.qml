import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

Rectangle {
    id : commLine;
    height : 35
    color : "#DDDDDD"
    TextField {
        id: t3
        anchors.leftMargin: 5
        placeholderText: "place holder script command"
        anchors.right: parent.right
        width : parent.width
        height : 25
        anchors.verticalCenter: parent.verticalCenter
    }
}
