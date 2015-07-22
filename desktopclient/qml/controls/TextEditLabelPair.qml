import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global

Item {
    property string content
    property string labelText
    property int labelWidth
    property bool textCanBeCopied : false
    property int fontSize : 8
    height : 20

    Text {
        id : label
        height : parent.height
        width : labelWidth
        text : labelText
        font.bold: true
        font.pointSize: fontSize
    }
    TextField{
        id : textid
        anchors.left : label.right
        height : parent.height
        width : parent.width - label.width
        text : content
        font.pointSize: fontSize
        style: TextFieldStyle {
            background: Rectangle {
                radius: 2
                width : parent.width
                height: parent.height
                border.color: Global.edgecolor
                border.width: textid.readOnly ? 0: 1
                color : "transparent"
            }
        }
    }
}

