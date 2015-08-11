import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import WorkSpaceModel 1.0
import "../../Global.js" as Global

Column {
    width : parent.width
    height : parent.height / 4
    spacing : 3
    Text {
        text : qsTr("Data")
        height: Global.rowHeight
        width : parent.width
        font.bold: true
    }
    Rectangle {
        width : parent.width
        height : parent.height - 20
        color : Global.alternatecolor4
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

                    data.model = workspace.data
                }
            }
            GridView {
                anchors.fill: parent
                id : data
                cellWidth: 170
                cellHeight: 14
                clip : true
                cacheBuffer: 1000
                flow: GridView.FlowTopToBottom
                model : workspace ? workspace.data : null
                delegate : WorkSpaceDataDelegate{}

            }
        }
    }
}


