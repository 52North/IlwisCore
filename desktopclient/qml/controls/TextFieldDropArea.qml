import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import "../Global.js" as Global

Item {
    id : tfdaContainer
    property string filterkeys
    property string content
    property string objectid : ""
    property bool readOnly : true
    property var canUse
    property bool asName : true
    function getText() {
        var temptxt = textid.text
        if ( temptxt.indexOf("/") != -1) // already full path, so we take this
            return temptxt;
        return content
    }

    DropArea{
        anchors.fill: parent
        onDropped : {
            if ( canUse){
                if ( canUse(drag.source.ilwisobjectid)){
                    content = drag.source.message
                    textid.text = asName ? mastercatalog.getName(drag.source.ilwisobjectid) : mastercatalog.getUrl(drag.source.ilwisobjectid)
                    objectid = drag.source.ilwisobjectid
                }
            }else {
                content = drag.source.message
                textid.text = asName ? mastercatalog.getName(drag.source.ilwisobjectid) : mastercatalog.getUrl(drag.source.ilwisobjectid)
            }
        }
        TextField{ id : textid;
            text : content;
            readOnly : readOnly
            width : parent.width
            height : parent.height
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
}

