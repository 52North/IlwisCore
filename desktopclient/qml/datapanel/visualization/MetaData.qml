import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import LayersView 1.0
import "../../workbench/propertyform" as MetaData
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base

Item {
    id : metatdata
    width : 210
    height : parent.height
    property alias coverage : overview
    property alias mouseActions : mouseActions
    property LayerManager manager
    property bool drawerActive : false

    onDrawerActiveChanged: {
        overview.active = drawerActive
    }

    signal zoomEnded(string envelope)

    function iconSource(name) {
        if ( name === "")
            name = "redbuttonr.png"
        var iconP = "../../images/" + name
        return iconP

    }

    function addDataSource(filter, sourceName, sourceType){
        if ( coverage){
            coverage.addCommand("adddrawer(" + coverage.viewerId + ","+ sourceName + "," + filter + "," + sourceType + ")")
            coverage.update()
        }
    }

    function transfer(datapanel){
        var layers = datapanel.manager.layers;
        for(var i =1; i < layers.length; i++){  // start at 1 because the first layer is always the global layer, is there by default so we skip it
            var expr = "adddrawer(" + coverage.viewerId + ","+ layers[i].name + ",\"itemid=" + layers[i].id + "\"," + layers[i].typeName + ")"
            coverage.addCommand(expr)
        }
        coverage.update()
    }

    function newZoomExtent(newenvelope){
        var env = {envelope : newenvelope, preserveaspectration : false}
        overview.setAttribute("selectiondrawer", env )
        mdspatialinfo.zoomEnvelope(newenvelope)
        overview.update()
    }

    function entireMap() {
        if ( metatdata.manager && layerview.manager){
            var envelope = renderer.attributeOfDrawer("rootdrawer","coverageenvelope")
            var env = {envelope : envelope, preserveaspectration : false}
            overview.setAttribute("selectiondrawer", env )
            mdspatialinfo.zoomEnvelope(envelope)
            overview.update()
        }
    }

    Connections {
        target: mouseActions
        onZoomEnded :{
            layersmeta.endZoom(envelope)
        }
    }


    Row {
        width: parent.width
        height : parent.height
        spacing : 3
        Rectangle {
            id : layerContainer
            width : 180
            height : parent.height
            color : Global.alternatecolor2
            border.color: "lightgrey"
            border.width: 1
            Rectangle {
                id : layersLabel
                width : 180
                height : 18
                color : Global.palegreen
                Text{
                    text : qsTr("Layers")
                    font.bold : true
                    x : 5
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
            MetaDataLayerList{
                anchors.top : layersLabel.bottom
                height : parent.height - layersLabel.height
                width : parent.width
            }


        }
        Rectangle{
            width : parent.width - layerContainer.width - 8
            height : metatdata.height
            color : Global.alternatecolor3



            MetaDataSpatialInfo{
                id : mdspatialinfo
                anchors.leftMargin: 2
                width : 250
                height : parent.height
            }
            Rectangle {
                id : overLabel
                width : overv.width
                height : 18
                anchors.left: mdspatialinfo.right
                anchors.top : parent.top
                color : Global.palegreen
                Text{
                    text : qsTr("Overview")
                    font.bold : true
                    x : 5
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
            Item {
                id : overv
                anchors.left : mdspatialinfo.right
                anchors.top : overLabel.bottom
                anchors.right: parent.right
                height : parent.height - overLabel.height

                OverviewExtentToolbar{
                    id : buttons
                    anchors.right: viewcontainer.left
                    height : parent.height

                }
                Rectangle{
                    id : viewcontainer
                    width :parent.width  - buttons.width
                    height : parent.height
                    border.width: 1
                    border.color: "lightgrey"
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: 3
                    function entireMap() {
                        if ( layersmeta.currentIndex == 2){
                            overview.addCommand("setviewextent("+ overview.viewerId + ",entiremap)");
                            overview.update()
                        }
                    }

                    onWidthChanged: {
                        entireMap()
                    }
                    onHeightChanged: {
                        entireMap()
                    }

                    onVisibleChanged: {
                        if (viewcontainer.visible)
                            entireMap()
                    }

                    OverViewDrawer{
                        id: overview
                        anchors.fill: parent

                        Component.onCompleted: {
                            mouseActions.mapScrollers = layers.maparea().parent.mapScrollers
                            entireMap()
                        }
                    }


                    LayerExtentMouseActions{
                        id : mouseActions
                        layerManager: manager
                        drawer : overview
                        linkedDrawer: renderer
                        hasPermanence: true
                        panningDirection: Global.panningAlong
                    }

                }
            }

        }
    }
}
