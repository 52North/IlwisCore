import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import "../../controls" as Controls


Item {
    Rectangle {
        width: parent.width - 5
        height: parent.height
        x : 5
        y : 5
        border.width: 1
        border.color : Global.edgecolor

        Column {
            id : buttonRow
            width : 200
            height : parent.height - 30
            spacing : 6
            property int buttonWidth : 180

            Controls.WideButton{
                image : "../images/openmult20.png"
                label : qsTr("Open selected coverages\nin seperate panels")
                width : buttonRow.buttonWidth
                height : 40
                onClicked: {
                    var ids = mastercatalog.selectedIds()
                    var idlist = ids.split("|")
                    for(var i=0; i < idlist.length; ++i){
                        showObject(idlist[i]);
                    }
                }
            }

            Controls.WideButton{
                image : "../images/opensingle20.png"
                label : qsTr("Open selected coverages\nin selected panel")
                width : buttonRow.buttonWidth
                height : 40
            }
            Controls.WideButton{
                image : "../images/openfloat20.png"
                label : qsTr("Open selected coverages\nin floating panels")
                width : buttonRow.buttonWidth
                height : 40
            }
        }
        Column {
            width : 200
            anchors.left: buttonRow.right
            spacing : 6
            Controls.WideButton{
                image : "../images/mapview.png"
                label : qsTr("Generate previews\nof selected coverages")
                width : buttonRow.buttonWidth
                height : 40
            }
            Column {
                height : Global.rowHeight * 3.2
                width : buttonRow.buttonWidth
                spacing : 2
                Controls.WideButton{
                    image : "../images/text20.png"
                    height : 40
                    width : buttonRow.buttonWidth
                    label: qsTr("Add keywords to\nselected items")
                }
                Row {
                    height : Global.rowHeight
                    width : buttonRow.buttonWidth
                    TextField{
                        width :parent.width - 40
                        height : parent.height
                    }
                    Button {
                        height : parent.height
                        width : 40
                        text : qsTr("Aplly")
                    }
                }
            }
        }
    }

}

