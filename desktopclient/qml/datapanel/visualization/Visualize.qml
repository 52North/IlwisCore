import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import GeoDrawer 1.0

Item {
    anchors.fill: parent

    function addSource(sourceUrl, sourceType){
        drawer.addDataSource(sourceUrl, sourceType)
        layertools.model = drawer.layers
    }

    SplitView {

        anchors.fill: parent
        orientation: Qt.Horizontal
        ListView {
            id : layertools
            width : 170
            height : parent.height
            delegate: Controls.CollapsiblePanel{
                id : layersdelegate
                width : parent.width
                titleText: name
                headerHeight: 20
                panelHeight: 200
                state : "collapsed"
                headerColor: Global.alternatecolor1

                Rectangle{
                    width : parent.width
                    height : parent.height
                    color : "red"
                    parent : layersdelegate.expandableArea
                }
            }
            clip : true


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
