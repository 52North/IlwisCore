import QtQuick 2.0

Item {
    property string content
    property string labelText
    property int labelWidth
    property bool textCanBeCopied : false
    property int fontSize : 10

    Text {
        id : label
        height : parent.height
        width : labelWidth
        text : labelText
        font.bold: true
        font.pointSize: fontSize
    }
    Text{
        anchors.left : label.right
        height : parent.height
        width : parent.width - label.width
        text : content
        font.pointSize: fontSize
    }
}

