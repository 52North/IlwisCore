import QtQuick 2.0

BorderImage {
    property string headerImage
    property string headerText

    id : title
    height : 30
    anchors.left: parent.left
    width : parent.width

    border { left: 15; top: 0; right: 15; bottom: 0 }
    smooth : true
    y : 5
    source : "../images/headerblueCS1.png"
    Rectangle {
        id : idBackground
        anchors.verticalCenter: parent.verticalCenter
        x : 5
        width: 24
        height : 24
        color : "white"
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
