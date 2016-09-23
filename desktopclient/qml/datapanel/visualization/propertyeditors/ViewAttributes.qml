import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global


Item {
    width : Math.min(300,parent ? Math.min(parent.width,500) : 300)
    height : Global.rowHeight + 20
    property var editor
    Button {
        x : 10
        y : 10
        text : qsTr("View Attributes")
        onClicked: {
            var info = editor.attributeTable
            if ( info.length == 2){
                var filter = "itemid=" + info[0]
                // try to find a suitable data pane for it
                datapanesplit.newPanel(filter, "table",info[1],"other")
            }
        }
    }
}
