import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "./propertyeditors" as DisplayOptions
import UIContextModel 1.0
import LayerManager 1.0
import ".." as DataPanel
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base


Item {
    id : viewmanager

    property var renderer

    signal zoomEnded(string envelope)

    Layout.minimumHeight: 22

    function addDataSource(filter, sourceName, sourceType){
        if ( filter.indexOf("=") !== -1){
            filter = "\"" + filter + "\""
        }
        var tab = layersmeta.getTab(2)

        if ( tab && tab.item ){
            tab.item.addDataSource(filter, sourceName, sourceType)
        }
    }

    function transfer(datapanel){
        var tab = layersmeta.getTab(2)
        if ( tab){
            tab.item.transfer(datapanel)
        }
    }

    function entireMap() {
        var tab = layersmeta.getTab(2)
        if ( tab){
            tab.item.entireMap()
        }
    }

     function newZoomExtent(newenvelope){
         var tab = layersmeta.getTab(2)
         if ( tab){
             tab.item.newZoomExtent(newenvelope)
         }
     }


    Component {
        id : displayOptions
        DisplayOptions.LayerPropertyManagement{}
    }

    Component {
        id : layersinfo
        LayersInfo{
        }
    }

    Component{
        id : metadata
        MetaData{
        }
    }



    TabView{
        id : layersmeta
        anchors.fill: parent
        tabPosition: Qt.BottomEdge
        onCurrentIndexChanged: {
            var tab= getTab(2)
            tab.item.drawerActive = currentIndex == 2

        }

        function tabClicked(index){
            if ( currentIndex === index){
                if ( viewmanager.height <= 60){
                    layers.state = "visible"
                }
                else{
                    layers.state = ""
                    layers.state = "invisible"
                }
            }

            currentIndex = index
        }

        function endZoom(envelope) {
            zoomEnded(envelope)
        }

        Component.onCompleted: {
            var tab =addTab(qsTr("Display Options"), displayOptions)
            tab.active = true
            tab.item.renderer = renderer

            addTab(qsTr("Layers Info"), layersinfo)
            tab = addTab(qsTr("Metadata"), metadata)
            tab.active = true // we need to be active as layers maybe added to it
        }

        style: DataPanel.ButtonBarTabViewStyle{}
    }



}

