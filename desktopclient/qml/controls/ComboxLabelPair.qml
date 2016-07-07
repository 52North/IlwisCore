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
    property alias currentIndex : textid.currentIndex
    height : 20

    signal indexChanged()

    function select(name){
        var index = -1
        for(var i=0; i < textid.model.length; ++i){
            if ( textid.model[i] === name)    {
                index = i
                break
            }
        }

        textid.currentIndex = index
    }

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

        onCurrentIndexChanged: {
            indexChanged()
        }
    }
}



