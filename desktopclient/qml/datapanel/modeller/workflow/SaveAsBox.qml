import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global

Controls.DropableItem {
    id : dropContainer
    width : 400
    maxHeight: 40
    Rectangle {
        anchors.fill: parent
        color : "white"
        border.width: 1
        border.color: Global.edgecolor
        radius: 2

        Controls.TextEditLabelPair{
            id :newname
            labelText: qsTr("New name")
            labelWidth: 100
            width : parent.width - applybutton.width - 14
            x : 5
            y : 7
        }
        Button {
            id :applybutton
            anchors.top : newname.top
            anchors.left: newname.right
            anchors.leftMargin: 2
            text : qsTr("Copy")
            width : 60
            height : newname.height
            onClicked: {
                var url = mastercatalog.currentCatalog.url
                if ( url.indexOf("file://") !== 0) {
                    url = workflow.rawUrl
                    if(url.indexOf("file://") !== 0)
                        return
                    //this url contains the name which we dont want
                    var index = url.lastIndexOf("/")
                    url = url.slice(0,index)
                }
                var obj = mastercatalog.id2object(workflow.id,0)
                obj.copy(url + "/" + newname.content,"workflow", "stream")
                obj.suicide()
                dropContainer.state = "invisible"
            }
        }
    }
}
