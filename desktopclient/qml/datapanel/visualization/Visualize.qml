import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import VisualizationManager 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import GeoDrawer 1.0

Item {
    id : displayOptions
    anchors.fill: parent
    objectName: uicontext.uniqueName()
    property VisualizationManager manager

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

         var iconP = "../../images/" + name
         return iconP
     }

    function addSource(sourceUrl, sourceType){
        drawer.addDataSource(sourceUrl, sourceType, manager)
        layertools.model = manager.layers
    }

    ToolBar{
        id : maptools
        width : parent.width
        height : 35
        Button {
            height : 25
            width : 25
            id : entireMap
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 2
            Image {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                source : iconsource("entiremap20.png")
            }
        }

        Button {
            height : 25
            width : 25
            id : refreshButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.left :entireMap.right
            anchors.rightMargin: 2
            Image {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                source : iconsource("refresh20.png")
            }
        }


        Button {
            height : 25
            width : 25
            id : panButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.left :refreshButton.right
            anchors.rightMargin: 2
            Image {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                source : iconsource("pan20.png")
            }
        }


        Button {
            height : 25
            width : 25
            id : zoominButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.left :panButton.right
            anchors.rightMargin: 2
            Image {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                source : iconsource("zoomin20.png")
            }
        }
        Button {
            height : 25
            width : 25
            id : zoomoutButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.left :zoominButton.right
            anchors.leftMargin: 2
            Image {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                source : iconsource("zoomout20.png")
            }
        }



    }

    SplitView {

        anchors.top: maptools.bottom
        width : parent.width
        height : parent.height - maptools.height
        orientation: Qt.Horizontal
        ListView {
            id : layertools
            objectName: uicontext.uniqueName()
            property int totalHeightLT : 0
            width : 170

            currentIndex: 0

            height : parent.height
            delegate: Controls.CollapsiblePanel{
                id : layersdelegate
                width : parent.width
                titleText: name
                headerHeight: 20
                panelHeight: 0
                state : "collapsed"
                headerColor: Global.alternatecolor1


                ListView {
                    id : propertyEditors
                    width : parent.width
                    height : 0
                    property int totalHeightPE : 0

                    delegate : Controls.CollapsiblePanel{
                        id : editorDelegate
                        width : parent.width
                        titleText: editorName
                        headerHeight: 18
                        panelHeight: defaultHeight
                        state : "collapsed"
                        headerColor: Global.alternatecolor3
                        arrowtype: "arrowdownlight.png"
                        boldfont: false
                        fontsize: 11
                        Component.onCompleted: {
                            var component = Qt.createComponent(qmlUrl);
                            if (component.status === Component.Ready){
                                component.createObject(editorDelegate.expandableArea);
                                propertyEditors.totalHeightPE = propertyEditors.totalHeightPE + defaultHeight
                            }
                        }
                    }
                    clip : true

                    parent : layersdelegate.expandableArea

                    Component.onCompleted: {
                        model = displayOptions.manager.layer(layertools.currentIndex).propertyEditors
                        panelHeight = propertyEditors.totalHeightPE
                        height = panelHeight
                        layertools.totalHeightLT = layertools.totalHeightLT + panelHeight
                    }

                }

            }
            clip : true

            Component.onCompleted: {
                 displayOptions.manager = uicontext.createVisualizationManager(objectName)
                 height = layertools.totalHeightLT

            }
            Component.onDestruction: {
                // TODO : remove current VisualizationManager
            }


        }

        Rectangle {
            anchors.left : layertools.right
            anchors.right : parent.right
            height : parent.height
            color : "white"

            GeoDrawer{
                id : drawer
                anchors.fill: parent
            }
        }
    }
}
