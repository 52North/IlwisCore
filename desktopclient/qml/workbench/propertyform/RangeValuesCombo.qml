import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls


Item {
    id : rangeValueCB
    property bool useDefaultRange : true

    height : 20
    width : parent.width

    function fromRange(rangedef){
        if ( rangedef === "")
            return "";
        var index = rangedef.indexOf(":")
        var itemstring = rangedef.substring(index + 1)
        var items = itemstring.split("|");
        return items;
    }


    Controls.ExpandableText {
        id : rangeText
        height : parent.height
        y : 0
        edittext: "Domain Items"
        editheight: parent.height
        state : "collapsed"
        backgroundColor: index  % 2 == 0 ? "#F7F9FC" : "#DCDCDC"
        width : parent.width
        panelHeight: 60

        Item {
            width : parent.width
            height : 60
            GridView{
                id : grid
                height :58
                cellWidth: 90
                cellHeight: 15
                width : parent.width
                model : fromRange(rangeDefinition(useDefaultRange, false,""))
                delegate : Component {
                    Text {
                        text : modelData
                        height : 15
                        width : grid.cellWidth - 2
                        font.pointSize: 8
                        font.italic: true
                        elide : Text.ElideMiddle
                    }
                }
            }

            parent : rangeText.expandableArea
        }
        onHeightChanged: {
            container.height = 22 + rangeText.height
        }

        clip : true

     }
}
