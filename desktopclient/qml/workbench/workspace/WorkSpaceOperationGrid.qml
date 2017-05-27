import QtQuick 2.0
import "../../Global.js" as Global

Column {
    width : parent.width
    height : parent.height / 4
    spacing : 3
    Text {
        text : qsTr("Operations")
        height: Global.rowHeight
        width : parent.width
        font.bold: true
    }
    Rectangle {
        width : parent.width
        height : parent.height - 20
        color : uicontext.workbenchBGColor
        border.width : 1
        border.color : Global.alternatecolor1
        DropArea {
            anchors.fill : parent
            onDropped: {
                if ( workspace){
                    // there migth still be a selection but this is overruled if the objectid is not in the list of selected ids
                    var parts = drag.source.ids.split('|')
                    var found = false
                    for(var i = 0; i < parts.length && found == false; ++i){
                        if ( parts[i] === drag.source.ilwisobjectid)
                            found = true
                    }

                    if (found){
                        workspace.addItems(drag.source.ids)
                    }else
                        workspace.addItems(drag.source.ilwisobjectid)

                    operations.model = workspace.operations
                }
            }
            GridView {
                anchors.fill: parent
                id : operations
                cellWidth: 170
                cellHeight: 14
                clip : true
                cacheBuffer: 1000
                flow: GridView.FlowTopToBottom
                model : workspace ? workspace.operations : null
                delegate : WorkSpaceDataDelegate{}

            }
        }
    }
}

