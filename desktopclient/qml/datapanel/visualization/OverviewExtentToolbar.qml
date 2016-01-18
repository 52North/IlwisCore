import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import "../../workbench/propertyform" as MetaData
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base

Rectangle {
    height : parent.height
    width : 27
    color : Global.alternatecolor1

    Action {
        id : zoomClicked
        onTriggered : {
            if ( metatdata.manager){
                if (!zoominButton1.checked ){
                    overview.addCommand("removedrawer(" + overview.viewerId + ",selectiondrawer,post)");
                    overview.update()
                }

                zoominButton1.checked = zoominButton1.checked ? false : true
                metatdata.manager.zoomInMode = zoominButton1.checked
            }
        }
    }

    Action {
        id : zoomOutClicked
        onTriggered : {
            if ( manager){
                var envelope = renderer.attributeOfDrawer("rootdrawer","zoomenvelope");
                Global.calcZoomOutEnvelope(envelope, layers, viewmanager)
            }
        }
    }

    Column{
        spacing : 2
        width : parent.width
        height : parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        MapExtentButton{
            id : panButton1
            icon : "pan20.png"
            onClicked: {
            }
        }

        MapExtentButton{
            id : zoominButton1
            icon : checked ? "zoomin20A.png" : "zoomin20.png"
            checkable: true
            checked: false
            action : zoomClicked
        }
        MapExtentButton{
            id : zoomoutButton1
            icon :"zoomout20.png"
            action : zoomOutClicked
        }
    }
}
