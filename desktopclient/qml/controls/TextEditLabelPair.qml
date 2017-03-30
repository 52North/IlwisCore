import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global

DropArea {
    property alias content : textid.text
    property string labelText
    property int labelWidth : 0
    property bool textCanBeCopied : false
    property int fontSize : 8
    property bool readOnly : false;
    property bool transparentBackgrond : true
    property var regexvalidator
    property bool boldLabel : true
    property int editWidth : Math.max(0,width - label.width - (keys.length > 0 ? 22 * keys.length : 0))
    property string ilwisobjectid // not always set
    height : Global.rowHeight
    width : parent.width

    signal contentEdited()

    Text {
        id : label
        height : parent.height
        width : labelWidth
        text : labelText
        font.weight: boldLabel ? Font.Bold : Font.DemiBold
        font.pointSize: fontSize
        elide: Text.ElideMiddle
        y :3
        clip :true
    }
    TextField{
        id : textid
        anchors.left : label.right
        height : parent.height
        width : editWidth
        text : content
        font.pointSize: fontSize
        readOnly : parent.readOnly

        property string oldText

        style: TextFieldStyle {
            background: Rectangle {
                radius: 3
                width : parent.width
                height: parent.height
                border.color: parent.enabled ? Global.edgecolor : "transparent"
                border.width: textid.readOnly ? 0: 1
                color : transparentBackgrond ? "transparent" : "white"
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

        onEditingFinished: {
            contentEdited()
        }
    }

    Row {
        width : childrenRect.width
        height : 20
        anchors.left: textid.right
        Repeater{
            model : keys.length
            Button{ width : 20;
                height:20

                checkable : true
                checked : false
                visible: keys.length > 0
                enabled: visible
                onClicked : {
                    if ( keys.length > 0){
                        var typeName = keys[index]
                        mastercatalog.currentCatalog.filterChanged(typeName + "|exclusive" , checked)
                    }
                }
                Image{anchors.centerIn : parent;width : 14; height:14
                    source: keys.length > 0 ? "../images/" + uicontext.type2icon(keys[index]) : ""
                    fillMode: Image.PreserveAspectFit
                }
            }
        }
    }

    onDropped : {
        content = drag.source.message
        if ( 'ilwisobjectid' in drag.source){
            ilwisobjectid = drag.source.ilwisobjectid
        }
    }
}

