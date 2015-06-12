import QtQuick 2.0

Item {
    property string content
    property string labelText
    property int labelWidth
    property bool textCanBeCopied : false

    Text {
        id : label
        height : parent.height
        width : labelWidth
        text : labelText
        font.bold: true
    }
    Text{
        anchors.left : label.right
        height : parent.height
        width : parent.width - label.width
        text : content
    }
}

