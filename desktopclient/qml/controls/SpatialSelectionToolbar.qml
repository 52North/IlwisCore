import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import "../controls" as Controls
import "../Global.js" as Global
import "../.." as Base

Rectangle {
    height : parent.height
    width : 27
    color : Global.alternatecolor1

    Action {
        id : zoomClicked
        onTriggered : {
            if ( worldmapcontainer.manager){
                if (!zoominButton1.checked ){
                    worldmap.addCommand("removedrawer(" + worldmap.viewerId + ",selectiondrawer,post)");
                    worldmap.update()
                }

                zoominButton1.checked = zoominButton1.checked ? false : true
                worldmapcontainer.manager.zoomInMode = zoominButton1.checked
                console.debug(zoominButton1.checked)
            }
        }
    }

    Action {
        id : zoomOutClicked
        onTriggered : {
            if ( worldmapcontainer.manager){
                var envelope = worldmap.attributeOfDrawer("rootdrawer","zoomenvelope");
                Global.calcZoomOutEnvelope(envelope, worldmap, worldmapcontainer.manager)
            }
        }
    }

    Column{
        spacing : 2
        width : parent.width
        height : parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        Controls.MapExtentButton{
            id : panButton1
            icon : "entiremap20.png"
            onClicked: {
                worldmap.addCommand("setviewextent("+ worldmap.viewerId + ",entiremap)");
                worldmap.update()
                currentCatalog.spatialFilter = ""
            }
        }

        Controls.MapExtentButton{
            id : zoominButton1
            icon : checked ? "zoomin20A.png" : "zoomin20.png"
            checkable: true
            checked: false
            action : zoomClicked
        }
        Controls.MapExtentButton{
            id : zoomoutButton1
            icon :"zoomout20.png"
            action : zoomOutClicked
        }
    }
}


