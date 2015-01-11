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


    function addSource(sourceUrl, sourceType){
        drawer.addDataSource(sourceUrl, sourceType)
        layertools.manager.addDataSource(sourceUrl, sourceType)
        layertools.model = layertools.manager.layers
    }

    SplitView {

        anchors.fill: parent
        orientation: Qt.Horizontal
        ListView {
            id : layertools
            objectName: uicontext.uniqueName()

            width : 170
            property VisualizationManager manager
            currentIndex: 0

            height : parent.height
            delegate: Controls.CollapsiblePanel{
                id : layersdelegate
                width : parent.width
                titleText: name
                headerHeight: 20
                panelHeight: 200
                state : "collapsed"
                headerColor: Global.alternatecolor1


                ListView {
                    width : parent.width
                    height : 100

                    delegate : Controls.CollapsiblePanel{
                        id : editorDelegate
                        width : parent.width
                        titleText: editorName
                        headerHeight: 18
                        panelHeight: 100
                        state : "collapsed"
                        headerColor: Global.alternatecolor3
                        arrowtype: "arrowdownlight.png"
                        boldfont: false
                        fontsize: 11
                        Component.onCompleted: {
                            var component = Qt.createComponent("RepresentationProperties.qml");
                            if (component.status === Component.Ready)
                                component.createObject(editorDelegate.expandableArea);
                        }
                    }
                    clip : true

                    parent : layersdelegate.expandableArea

                    Component.onCompleted: {
                        model = layertools.manager.layer(layertools.currentIndex).propertyEditors
                    }

                }

            }
            clip : true

            Component.onCompleted: {
                 manager = uicontext.createVisualizationManager(objectName)
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
