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

    Action {
        id : zoomClicked
        onTriggered : {
            if ( manager){
                manager.zoomInMode = !manager.zoomInMode
                zoominButton.imageSource = iconsource(manager.zoomInMode ? "zoomin20A.png" : "zoomin20.png")
                zoominButton.checked = !zoominButton.checked
            }
        }

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
            action : zoomClicked
            checkable: true
            checked: false
            property string imageSource :  "zoomin20.png"
            Image {
                id : zoomimage
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                source : iconsource(zoominButton.imageSource)
            }
        }
        Button {
            height : 25
            width : 25
            id : zoomoutButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.left :zoominButton.right
            anchors.leftMargin: 2
            checkable: true
            checked: false
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
                            if ( qmlUrl !== ""){
                                var component = Qt.createComponent(qmlUrl);
                                if (component.status === Component.Ready){
                                    component.createObject(editorDelegate.expandableArea);
                                    propertyEditors.totalHeightPE = propertyEditors.totalHeightPE + defaultHeight
                                }
                            }
                        }
                    }
                    clip : true

                    parent : layersdelegate.expandableArea

                    Component.onCompleted: {
                        model = displayOptions.manager.layer(index).propertyEditors
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
            DropArea {
                anchors.fill : parent
                onDropped: {
                    console.log (drag.source.ilwisobjectid);
                    var resource = mastercatalog.id2Resource(drag.source.ilwisobjectid)
                    addSource(resource.url, resource.typeName)
                }
                GeoDrawer{
                    id : drawer
                    anchors.fill: parent

                    MouseArea {
                        id : mapArea
                        anchors.fill: parent
                        onPressed: {
                            if ( manager.zoomInMode ){
                                if ( !manager.hasSelectionDrawer){
                                    var position = {currentx: mouseX, currenty:mouseY}
                                    drawer.addDrawer("SelectionDrawer", position)
                                    manager.hasSelectionDrawer = true
                                }
                            }
                        }
                        onPositionChanged: {
                            if ( manager.hasSelectionDrawer){
                                var position = {currentx: mouseX, currenty:mouseY}
                                drawer.setAttribute("SelectionDrawer", position)
                                drawer.update()
                            }
                        }
                        onReleased: {
                            if ( manager.zoomInMode && manager.hasSelectionDrawer){
                                drawer.removeDrawer("SelectionDrawer",true)
                                manager.hasSelectionDrawer = false
                                drawer.update()
                            }
                        }
                    }
                }
            }
        }
    }
}
