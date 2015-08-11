import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global

Item {
    property string content
    property string labelText
    property int labelWidth
    property bool textEditable : false
    property int fontSize : 8
    property bool readOnly : false;
    property var itemModel
    property alias comboText : textid.currentText
    property string role
    height : 20

    Text {
        id : label
        height : parent.height
        width : labelWidth
        text : labelText
        font.bold: true
        font.pointSize: fontSize
        y :3
    }
    ComboBox{
        id : textid
        anchors.left : label.right
        height : parent.height
        width : parent.width - label.width
        model : itemModel
        editable : textEditable
        textRole: role
    }
}



