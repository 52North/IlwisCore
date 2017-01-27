import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global

Item {
    id : papa
    property alias content : textid.text
    property string labelText
    property int labelWidth
    property bool textCanBeCopied : false
    property int fontSize : 8
    property alias readOnly : textid.readOnly
    property alias lineCount: textid.lineCount
    property alias richText : textid.textFormat

    height : 20

    Text {
        id : label
        height : parent.height
        width : labelWidth
        text : labelText
        font.bold: true
        font.pointSize: fontSize
    }
    TextArea{
        id : textid
        anchors.left : label.right
        height : parent.height
        width : parent.width - label.width
        text : content
         font.pointSize: fontSize
        readOnly: papa.readOnly

    }
}

