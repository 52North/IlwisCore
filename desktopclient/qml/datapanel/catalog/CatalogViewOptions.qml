import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import "../../controls" as Controls


Item {
    width: catalogoptions.width
    height: catalogoptions.height
    Rectangle {
        width: 200
        height: 110
        x : 5
        y : 5
        border.width: 1
        border.color : Global.edgecolor

        Column {
            width : parent.width - 25
            height : parent.height - 30
            anchors.centerIn: parent
            spacing : 6

            Controls.WideButton{
                image : "../images/openmult20.png"
                label : qsTr("Open selected coverages\nin seperate panels")
                width : 200
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
                width : 200
                height : 40
            }
            Controls.WideButton{
                image : "../images/openfloat20.png"
                label : qsTr("Open selected coverages\nin floating panels")
                width : 200
                height : 40
            }
        }
    }

}

