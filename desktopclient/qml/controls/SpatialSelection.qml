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
    height : 30 + operList.height
    width : 370
    Controls.CollapsiblePanel {
        y : 4
        x : 8
        id : worldmapcontainer
        width : parent.width
        titleText: qsTr("Spatial Selection")
        headerHeight: 20
        panelHeight: 270
        state : "collapsed"
        objectName: uicontext.uniqueName()
        property LayerManager manager
        headerColor: Global.alternatecolor5



        Rectangle{
            id : operList
            width : parent.width
            height : parent.height
            parent : worldmapcontainer.expandableArea
            border.width : 1
            border.color : Global.edgecolor

            SpatialSelectionToolbar{
                id : buttons
                anchors.right: worldmap.left
            }

            LayersView{
                id : worldmap
                width : parent.width - 30
                height : worldmapcontainer.panelHeight - 4
                x : 28
                y : 2
                Connections{
                    target : mouseActions
                    onZoomEnded : {
                        currentCatalog.spatialFilter = envelope
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
            }

        }

        clip : true
    }


    Component.onCompleted: {
       worldmapcontainer.manager = uicontext.createLayerManager(objectName)
       worldmap.setManager(worldmapcontainer.manager)
    }
}

