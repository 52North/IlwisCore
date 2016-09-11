import QtQuick 2.0
import "../Global.js" as Global

BorderImage {
    source: "../images/tab_big_green.png"
    border.left: 5; border.top: 5
    border.right: 5; border.bottom: 5
    property string headerImage
    property string headerText

    id : title
    height : 30
    anchors.left: parent.left
    width : parent.width - 4
    anchors.leftMargin: 4
    Image {
        id : idBackground
        anchors.verticalCenter: parent.verticalCenter
        x : 5
        width: 24
        height : 24
        source: headerImage
        fillMode: Image.PreserveAspectFit
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
