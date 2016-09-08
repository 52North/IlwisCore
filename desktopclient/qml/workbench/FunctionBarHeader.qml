import QtQuick 2.0
import "../Global.js" as Global

Rectangle {
    property string headerImage
    property string headerText

    id : title
    height : 30
    anchors.left: parent.left
    width : parent.width
    color : Global.darkgreen

    y : 5
    Rectangle {
        id : idBackground
        anchors.verticalCenter: parent.verticalCenter
        x : 5
        width: 24
        height : 24
        color : Global.middlegreen
        Image {
            id : icon
            source: headerImage
            anchors.fill: parent
            anchors.margins: 2
            fillMode: Image.PreserveAspectFit
        }
    }
    Text {
        x : 39
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        text : headerText
        font.pointSize: 12
        font.bold: true
        color : "white"
    }
}
