import QtQuick 2.0

Rectangle {
    width: 200
    height: 80
    property string name
    Image {
        id : box
        anchors.fill: parent
        source : iconsource("operationitem.png")
    }
    Text{
        anchors.top : box.top
        anchors.topMargin: 9
        width : box.width
        x : 15
        text : name
        font.bold : true

    }
}

