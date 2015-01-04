import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import GeoDrawer 1.0

Item {
    anchors.fill: parent
    property string sourceUrl
    property string sourceType

    onSourceUrlChanged: {
        if ( sourceType != ""){
            drawer.addDataSource(sourceUrl, sourceType)
        }
    }

    onSourceTypeChanged: {
        if ( sourceUrl != ""){
          drawer.addDataSource(sourceUrl, sourceType)
        }
    }

    SplitView {

        anchors.fill: parent
        orientation: Qt.Horizontal
        Rectangle {
            id : layertools
            width : 170
            height : parent.height
            //color : "blue"

        }

        Rectangle {
            anchors.left : layertools.right
            anchors.right : parent.right
            height : parent.height
            color : "white"

            GeoDrawer{
                id : drawer
                anchors.fill: parent
//                x : 25
//                y : 75
//                width : 400
//                height : 400

                Component.onCompleted:  {
                    addDataSource(sourceUrl, sourceType)
                }
//                Rectangle {
//                    anchors.fill: parent
//                    anchors.margins: -2
//                    color : "transparent"
//                    border.width: 2
//                    border.color : "blue"
//                }
            }
        }
    }
}
