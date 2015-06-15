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
                layer
                var envelope = renderer.attributeOfDrawer("rootdrawer","zoomenvelope");
                if ( envelope !== ""){
                    var parts = envelope.split(" ")
                    var x1 = parseFloat(parts[0])
                    var y1 = parseFloat(parts[1])
                    var x2 = parseFloat(parts[3])
                    var y2 = parseFloat(parts[4])
                    var dx = 0.707 * Math.abs(x1 - x2)
                    var dy = 0.707 * Math.abs(y1 - y2)
                    var nx1 = (x2 + x1) / 2.0 - dx
                    var nx2 = (x2 + x1) / 2.0 + dx
                    var ny1 = (y2 + y1) / 2.0 - dy
                    var ny2 = (y2 + y1) / 2.0 + dy
                    var newenvelope = nx1 + " " + ny1 + " " + nx2 + " " + ny2
                    layers.newExtent(newenvelope)
                                        console.debug("a", envelope, newenvelope)
                    viewmanager.newZoomExtent(newenvelope)
                }
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
                console.debug("todo")
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
