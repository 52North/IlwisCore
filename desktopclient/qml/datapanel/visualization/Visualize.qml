import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import GeoDrawer 1.0

Item {
    anchors.fill: parent

    function addSource(sourceUrl, sourceType){
        drawer.addDataSource(sourceUrl, sourceType)
    }

    SplitView {

        anchors.fill: parent
        orientation: Qt.Horizontal
        ListView {
            id : layertools
            width : 170
            height : parent.height


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
