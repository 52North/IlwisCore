import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import TabModel 1.0
import "./propertyeditors" as LayerManagement
import "../../controls" as Controls
import "../../Global.js" as Global
import "../../pubsub.js" as PubSub
//import GeoDrawer 1.0


Item {
    id : layerview
    width: parent.width
    height : parent.height
    property TabModel tabmodel
    property string panelType : "mappanel"
    property var createParameters : []
    property alias viewmanager : viewmanager

    objectName: uicontext.uniqueName()
    property LayerManager manager
    property bool canSeparate : true

    property string selectiondrawertopic : "main"
    property string selectiondrawertopicoverview : "overview"


    function subscribe (topic, func) {
        return PubSub.subscribe( topic, func );
    }

    function publish(topic, data) {
        return PubSub.publish(topic, data);
    }

    function unsubscribe(topic, token) {
        PubSub.unsubscribe( token );
    }

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../../images/" + name
        return iconP
    }

    function addDataSource(sourceUrl, sourceName, sourceType){
        if ( sourceUrl !== ""){
            var id = layers.addDataSource(sourceUrl, sourceName, sourceType)
            viewmanager.addDataSource(sourceUrl, sourceName, sourceType)
            createParameters = [sourceUrl, sourceName, sourceType]
            return id
        }
        return -1
    }

    function transfer(datapanel){
        layers.transfer(datapanel)
        viewmanager.transfer(datapanel)        
    }

    Action {
        id : zoomClicked
        onTriggered : {
            if ( manager){
                manager.zoomInMode = !manager.zoomInMode
                manager.panningMode = false
            }
        }
    }

    Action {
        id : panningClicked
        onTriggered : {
            if ( manager){
                manager.panningMode = !manager.panningMode
                manager.zoomInMode = false
            }
        }
    }


    Action {
        id : entireClicked
        onTriggered : {
            manager.zoomInMode = false
            layers.entireMap()
            viewmanager.entireMap()
        }
    }
    Action {
        id : refreshClicked
        onTriggered: {

        }
    }
    Action {
        id : zoomOutClicked
        onTriggered : {
            if ( manager){
                var envelope = layers.drawer().attributeOfDrawer("rootdrawer","zoomenvelope");
                Global.calcZoomOutEnvelope(envelope, layers, viewmanager,0.707)
            }
        }
    }
    Controls.LayerExtentsToolbar{
        id : maptools

        LocationBar{
            renderer: layers.drawer()
        }
    }

    SplitView {
        anchors.top : maptools.bottom
        width : parent.width
        orientation: Qt.Vertical
        height : parent.height - maptools.height

        Connections {
            target: layers
            onZoomEnded :{
                viewmanager.newZoomExtent(envelope)
            }
        }
        /*Connections {
            target: viewmanager
            onZoomEnded :{
                viewmanager.newZoomExtent(envelope)
            }
        }*/
        Layers{

            width : parent.width
            height : parent.height - Global.actionBarMaxHeight
            id : layers
            states: [
                State { name: "visible"

                    PropertyChanges {
                        target: layers
                        height : parent.height -  Global.actionBarMaxHeight
                    }
                },
                State {
                    name : "invisible"
                    PropertyChanges {
                        target: layers
                        height : parent.height - maptools.height + 10
                    }
                }

            ]
            transitions: [
                Transition {
                    NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
                }
            ]

        }
        ViewManager{
            id : viewmanager
            height : Global.actionBarHeight
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.right: parent.right
            renderer: layers.drawer()
        }
    }

    Component.onCompleted: {
        manager = uicontext.createLayerManager(objectName)
        layers.setManager(manager)

    }
    Component.onDestruction: {
        // TODO : remove current VisualizationManager
    }

}
