import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import "../Global.js" as Global

Item {
    id : tfdaContainer
    property string filterkeys
    property string currentText
    property bool readOnly : true
    property var canUse
    function getText() {
        var temptxt = textid.text
        if ( temptxt.indexOf("/") != -1) // already full path, so we take this
            return temptxt;
        return currentText
    }


    DropArea{
        anchors.fill: parent
        onDropped : {
            if ( canUse){
                if ( canUse(drag.source.ilwisobjectid)){
                    currentText = drag.source.message
                    textid.text = mastercatalog.getName(drag.source.ilwisobjectid)
                }
            }else {
                currentText = drag.source.message
                textid.text = mastercatalog.getName(drag.source.ilwisobjectid)
            }
        }
        TextField{ id : textid;
            text : currentText;
            readOnly : readOnly
            width : parent.width
            height : parent.height
            style: TextFieldStyle {
                background: Rectangle {
                    radius: 2
                    width : parent.width
                    height: parent.height
                    border.color: "#333"
                    border.width: textid.readOnly ? 0: 1
                    color : "transparent"
                }
            }
        }
    }
}

