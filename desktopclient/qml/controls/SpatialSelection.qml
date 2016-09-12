import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global
import "../controls" as Controls
import "../datapanel/visualization" as MapTools
import LayersView 1.0
import UIContextModel 1.0
import LayerManager 1.0

Item {
    id : selector
    property var currentEnvelope


    onCurrentEnvelopeChanged: {
        if ( currentEnvelope === "entiremap"){
            worldmap.addCommand("setviewextent("+ worldmap.viewerId + ",entiremap)");
            worldmap.update()
            currentCatalog.spatialFilter = ""
        }else
            worldmap.newExtent(currentEnvelope)
    }
    Rectangle {
        id : mapLabel
        width : parent.width
        height : 18
        color : Global.palegreen
        Text{
            text : qsTr("Spatial Selection")
            font.bold: true
            x : 3
            anchors.verticalCenter: parent.verticalCenter
        }
    }


    Rectangle{
        id : operList
        width : parent.width
        height : 250
        anchors.top: mapLabel.bottom
        property LayerManager manager

        SpatialSelectionToolbar{
            id : buttons
            anchors.right: worldmap.left
        }

        LayersView{
            id : worldmap
            width : parent.width - 30
            height : 240
            x : 28
            y : 2
            Connections{
                target : mouseActions
                onZoomEnded : {
                    currentCatalog.filter("spatial", envelope)
                    currentEnvelope = envelope
                }
            }

            function newExtent(ext){
                addCommand("setviewextent("+ viewerId + "," + ext + ")");
                update()
            }
            Component.onCompleted: {
                var cmd = uicontext.worldmapCommand(viewerId)
                addCommand(cmd)
                setAttribute("GridDrawer", {"active" : false})
                cmd = "setlinecolor("+ viewerId + ", 0, black)";
                addCommand(cmd)
            }
            LayerExtentMouseActions{
                id : mouseActions
                layerManager: operList.manager
                drawer : worldmap
            }
            Controls.ToolTip{
                target: worldmap
                text : qsTr("Selects coverages within the indicated rectangle")
            }
        }

    }

    clip : true


    Component.onCompleted: {
        operList.manager = uicontext.createLayerManager(objectName)
        worldmap.setManager(operList.manager)
    }
}

