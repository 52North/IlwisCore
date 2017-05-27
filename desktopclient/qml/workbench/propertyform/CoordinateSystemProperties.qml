import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Rectangle {
    id : csycontainer
    width: parent.width
    height: childrenRect.height
    property int lineheight : 19
     color : uicontext.lightestColor

    function storeData() {
        if ( propertyForm.editable){
            setAttribute("coordinatesystem",line1.content)
        }
    }

    Controls.FilteredTextEditLabelPair{
        id : line1
        labelWidth: 120
        labelText: qsTr("Coordinate system")
        filterImage: "../images/csy20.png"
        filterType: "coordinatesystem"
        width : parent.width
        content: isAnonymous ? "internal" : coordinateSystemName
        readOnly: !propertyForm.editable
        y : 5
    }

    Controls.TextEditLabelPair{
        id : line2
        labelWidth: 120
        labelText: qsTr("Is Projected")
        width : parent.width
        content: isProjected ? qsTr("Yes") : qsTr("No")
        readOnly: true
        y : 5
        anchors.top : line1.bottom
    }
    Loader{
        id : projectionInfoLine
        height : isProjected ? lineheight : 0
        width : parent.width
        source : isProjected ? "ProjectionProperties.qml" : ""
        anchors.top : line2.bottom
    }
    Text { id : line3; text : qsTr("LatLon Envelope"); width: 120;anchors.top : projectionInfoLine.bottom; height : lineheight}
    Envelope {
        id : llenvelope
        envelope : getProperty("latlonenvelope")
        width : csycontainer.width - line3.width - 2
        isLatLon: true
        anchors.top : line3.bottom
    }

    Text { id : line4; text : qsTr("Envelope"); width: 120;anchors.top : llenvelope.bottom; height : lineheight }
//   Text { text : getProperty("envelope");  height : lineheight;width: parent.width - line4.width - 2; anchors.left: line4.right;anchors.top : llenvelope.bottom}
    Envelope {
        id : prjenvelope
        envelope : getProperty("envelope")
        width : csycontainer.width - line4.width - 2
        isLatLon: false
        anchors.top : line4.bottom
    }
    Image {
        source : "../../images/worldlines.PNG"
        anchors.top : prjenvelope.bottom
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
