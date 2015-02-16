import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../Global.js" as Global

Item {
    id : tfdaContainer
    property string filterkeys
    property string currentText
    function getText() {
        var temptxt = textid.text
        if ( temptxt.indexOf("/") != -1) // already full path, so we take this
            return temptxt;
        return currentText
    }

    function shortName(name){
        var index = name.indexOf("/")
        if ( index === -1)
            return name
        var parts = name.split("/");
        return parts[parts.length - 1];
    }

    DropArea{
        anchors.fill: parent
        onDropped : {
            currentText = drag.source.message
            textid.text = shortName(currentText)
        }
        TextField{ id : textid;
            text : currentText;
            readOnly : !propertyForm.editable
            width : parent.width
            height : parent.height
            style: TextFieldStyle {
                background: Rectangle {
                    radius: 2
                    width : parent.width
                    height: parent.height
                    border.color: "#333"
                    border.width: textid.readOnly ? 0: 1
                }
            }
        }
    }
}

