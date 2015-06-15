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
    property LayerManager manager

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

    Rectangle {
        id : layersLabel
        width : parent.width + 10
        height : 18
        color : Global.alternatecolor3
        Text{
            text : qsTr("Layers")
            font.weight: Font.DemiBold
            x : 5
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    Row {
        width: parent.width
        anchors.top: layersLabel.bottom
        anchors.topMargin: 2
        height : parent.height - layersLabel.height - 5
        spacing : 3
        Rectangle {
            id : layerContainer
            width : 180
            height : parent.height
            color : Global.alternatecolor2
            border.color: "lightgrey"
            border.width: 1
            MetaDataLayerList{
            }


        }
        Rectangle{
            width : parent.width - layerContainer.width - 8
            height : layerContainer.height
            MetaDataSpatialInfo{
                id : mdspatialinfo
                anchors.topMargin: 4
                anchors.leftMargin: 2
                anchors.left : parent.left
                anchors.right: buttons.left
                height : parent.height
            }

            OverviewExtentToolbar{
                id : buttons
                anchors.right: viewcontainer.left
            }
            Rectangle{
                id : viewcontainer
                width :parent.width / 2 - buttons.width
                height : parent.height - 4
                border.width: 1
                border.color: "lightgrey"
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 3
                function entireMap() {
                    var envelope = overview.attributeOfDrawer("rootdrawer","coverageenvelope");
                    if ( envelope !== ""){
                        overview.addCommand("setviewextent("+ overview.viewerId + "," + envelope + ")");
                    }
                    overview.update()
                }

                onWidthChanged: {
                    entireMap()
                }
                onHeightChanged: {
                    entireMap()
                }

                OverViewDrawer{
                    id: overview
                    anchors.fill: parent
                }


                LayerExtentMouseActions{
                    id : mouseActions
                    layerManager: manager
                    drawer : overview
                    linkedDrawer: renderer
                    hasPermanence: true
                }

            }

            border.color: "lightgrey"
            border.width: 1

        }
    }
}
