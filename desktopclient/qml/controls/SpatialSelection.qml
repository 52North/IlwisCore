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
    property var filterTarget
    clip : true


    onCurrentEnvelopeChanged: {
        if ( currentEnvelope === "entiremap"){
            worldmap.addCommand("setviewextent("+ worldmap.viewerId + ",entiremap)");
            worldmap.update()
            if ( filterTarget){
                if ( "spatialFilter" in filterTarget)
                    filterTarget.spatialFilter = ""
            }
        }else
            worldmap.newExtent(currentEnvelope)
    }



    Rectangle{
        id : worldmapcontainer
        width : parent.width
        height : 250
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
                    if ( filterTarget)
                        filterTarget.filter("spatial", envelope)
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
                layerManager: worldmapcontainer.manager
                drawer : worldmap
            }
            Controls.ToolTip{
                target: worldmap
                text : qsTr("Selects coverages within the indicated rectangle")
            }
        }

    }

    Component.onCompleted: {
        worldmapcontainer.manager = uicontext.createLayerManager(selector)
        worldmap.setManager(worldmapcontainer.manager)
    }
    states: [
        State { name: "invisible"

            PropertyChanges {
                target: selector
                height : 0
            }

        },
        State {
            name : "visible"
            PropertyChanges {
                target: selector
                height : 270
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation
            { properties: "height"; duration : 500 ; easing.type: Easing.OutCubic
            }

        }
    ]
}

