import QtQuick 2.0
import GeoDrawer 1.0

Rectangle {
    property string sourceUrl
    property string sourceType

    anchors.fill : parent
    color : "white"
    border.width: 2
    border.color: "red"
    GeoDrawer{
        x : 25
        y : 75
        width : 400
        height : 400

        Component.onCompleted:  {
            addDataSource(sourceUrl, sourceType)
        }
        Rectangle {
            anchors.fill: parent
            anchors.margins: -2
            color : "transparent"
            border.width: 2
            border.color : "blue"
        }
    }
}
