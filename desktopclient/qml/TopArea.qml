import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import QtQuick.Window 2.1
import "workbench" as WorkBench
import "datapanel" as DataPanel
import "controls" as Control
import MessageModel 1.0
import ResourceModel 1.0
import MasterCatalogModel 1.0
import QtGraphicalEffects 1.0
import "Global.js" as Global
import "controls" as Controls


Item {
    id : topRoot
    Item {
        id : topItem
        z: topRoot.z + 1
        width: parent.width
        height: parent.height - 6
        Rectangle {
            id : aArea
            width : parent.width
            height : parent.height - 6
            color : Global.darkestgreen
            Image {
                x : 4
                y : 4
                source : "images/ilwis_logo_txt.png"
            }
            Controls.ActionButtonV{
                width : 100
                height :20
                anchors.verticalCenter: parent.verticalCenter
                text : "Python console"
                anchors.right: parent.right
                anchors.rightMargin: 4
                backColor: Global.lightgreen

                onClicked: {
                    var id = uicontext.consoleScriptId()
                    bigthing.newCatalog("itemid=" + id, "script", "ilwis://internalcatalog/consolescript.py", "other")
                }

            }
        }

    }
    DropShadow {
        id: cmlShadow
        z: topRoot.z + 1
        anchors.fill: source
        cached: true
        verticalOffset: 4
        radius: 4
        samples: 12
        color: "#80000000"
        smooth: true
        source: topItem
    }

}
