import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../controls" as Controls
import "../Global.js" as Global

TextField{
    id : textid
   //anchors.left : label.right
    height : Global.rowHeight
    width : parent.width
    readOnly : false
    property var regexvalidator

    property string oldText

    style: TextFieldStyle {
        background: Rectangle {
            radius: 3
            width : parent.width
            height: parent.height
            border.color: parent.enabled ? Global.edgecolor : "transparent"
            border.width: textid.readOnly ? 0: 1
            color : "transparent"
        }
    }
    onTextChanged: {
        if ( regexvalidator){
            var regex = regexvalidator
            if (!regex.test(textid.text)){
                textid.text = textid.oldText
            }
            oldText = textid.text
        }
    }
}

