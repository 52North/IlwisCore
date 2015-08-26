import QtQuick 2.0
import "../../Global.js" as Global

Rectangle {
    width: parent.width
    height: 220
    property int lineheight : 19
     color : Global.formBackGround

    function storeData() {
        if ( propertyForm.editable){
        }
    }

    Text { y : 5; id : line1; text : qsTr("Coordinate system"); width: 120; font.bold: true;height : lineheight }
    Text { y : 5; text : coordinateSystemName;  height :40;width: parent.width - line1.width - 2; anchors.left: line1.right}
    Text { id : line2; text : qsTr("Is Projected"); width: 120; font.bold: true ;anchors.top : line1.bottom; height : lineheight}
    Text { text : isProjected ? qsTr("Yes") : qsTr("No");  height : lineheight;width: parent.width - line1.width - 2; anchors.left: line1.right;anchors.top : line1.bottom}
    Loader{
        id : projectionInfoLine
        height : isProjected ? lineheight : 0
        width : parent.width
        source : isProjected ? "ProjectionProperties.qml" : ""
        anchors.top : line2.bottom
    }
    Text { id : line3; text : qsTr("LatLon Envelope"); width: 120; font.bold: true ;anchors.top : projectionInfoLine.bottom; height : lineheight}
    Text { text : getProperty("latlonenvelope");  height : lineheight;width: parent.width - line3.width - 2; anchors.left: line3.right;anchors.top : projectionInfoLine.bottom}
    Text { id : line4; text : qsTr("Envelope"); width: 120; font.bold: true ;anchors.top : line3.bottom; height : lineheight}
    Text { text : getProperty("envelope");  height : lineheight;width: parent.width - line4.width - 2; anchors.left: line4.right;anchors.top : line3.bottom}
    Image {
        source : "../../images/worldlines.PNG"
        anchors.top : line4.bottom
        anchors.topMargin: 5
        x : 10
        Rectangle{
            id : horLine
            x : 0
            y : 0
            width : 0
            height : 1
            color : "red"
        }
        Rectangle{
            id : vertLine
            x : 0
            y : 0
            width : 1
            height : 0
            color : "red"
        }

        Component.onCompleted: {
            var center = getProperty("centerpixelboundingbox")
            if ( center === "")
                return

            var px = center.split(" ")[0]
            var py = center.split(" ")[1]

            horLine.y = py
            horLine.width = 300

            vertLine.x = px
            vertLine.height = 120
        }
    }

}
