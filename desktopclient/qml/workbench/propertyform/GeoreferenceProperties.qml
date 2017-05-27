import QtQuick 2.0
import "../../Global.js" as Global

Rectangle {
      color : uicontext.lightestColor
    function storeData() {
        if ( propertyForm.editable){
        }
    }

    property int lineheight : 19

    Text { id : line1; text : qsTr("Raster Size"); width: 120; font.bold: true ; height : lineheight}
    Text { id : dimText; text : dimensions;  height : lineheight; anchors.left: line1.right}

    Text { y : 5; id : line2; text : qsTr("Pixel Size"); width: 120; font.bold: true;height : lineheight;anchors.top : line1.bottom; }
    Text { y : 5; text : getProperty("pixelsize");  height :40;width: parent.width - line2.width - 2; anchors.left: line2.right; anchors.top : line1.bottom}

    Text { y : 5; id : line3; text : qsTr("Coordinate system"); width: 120; font.bold: true;height : lineheight;anchors.top : line2.bottom; }
    Text { y : 5; text : isAnonymous ? "internal" : coordinateSystemName;  height :40;width: parent.width - line3.width - 2; anchors.left: line3.right; anchors.top : line2.bottom}

    Text { id : line4; text : qsTr("Is Projected"); width: 120; font.bold: true ;anchors.top : line3.bottom; height : lineheight}
    Text { text : isProjected ? qsTr("Yes") : qsTr("No");  height : lineheight;width: parent.width - line4.width - 2; anchors.left: line4.right;anchors.top : line3.bottom}

}
