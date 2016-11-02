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
                panButton1.checked = false
            }
        }
    }

    Action {
        id : zoomOutClicked
        onTriggered : {
            if ( manager){
                var envelope = renderer.attributeOfDrawer("rootdrawer","zoomenvelope");
                console.log("OverviewExtentToolbar.qml: zoomOutClicked: envelope= " + envelope)
                Global.calcZoomOutEnvelope(envelope, layers, viewmanager,0.707)
            }
        }
    }

    Action {
        id : panningClicked
        onTriggered : {
            if ( manager){
                panButton1.checked = !panButton1.checked
                zoominButton1.checked = false
                manager.panningMode = !manager.panningMode
                manager.zoomInMode = false
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
            checkable: true
            checked: false
            icon : checked ? "pan20A.png" : "pan20.png"
            action : panningClicked
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
