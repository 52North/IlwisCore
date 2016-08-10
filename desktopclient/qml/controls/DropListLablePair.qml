import QtQuick 2.0
import "../Global.js" as Global

Column {
    width : parent.width
    height : childrenRect.height
    property alias labelText : title.text
    property alias areaheight : list.height
    property var items : []

    Text {
        id : title
        width : parent.width
        height : text != "" ? 18 : 0
    }
    Rectangle {
        width : parent.width
        height : 60
        border.width: 1
        border.color: Global.edgecolor
        DropArea {
            anchors.fill: parent
            ListView {
                id : list
                anchors.fill: parent
                model : items
                delegate {
                    Text {
                        text : modelData
                        width : parent.width
                        height : 18
                    }
                }
            }
            onDropped: {
                items.push(drag.source.message)
                list.model = items
            }
        }
    }

}
