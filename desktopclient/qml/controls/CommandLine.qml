import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

Rectangle {
    id : commLine;
    width : parent.width
    height : 32
    color : "#DDDDDD"
    TextField {
        id: t3
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.topMargin: 5
        anchors.bottomMargin: 5
        anchors.rightMargin: 40
        placeholderText: "This is a placeholder for a TextField"
        width: 200
    }
}
