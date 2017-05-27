import QtQuick 2.0
import "../../Global.js" as Global

Rectangle {
    width: parent.width
    height: 150
    property int lineheight : 19
    color : uicontext.lightestColor


    Text { y : 5; id : line1; text : qsTr("Georeference"); width: 120; font.bold: true;height : lineheight }
    Text { y : 5; text : geoReferenceName;  height :40;width: parent.width - line1.width - 2; anchors.left: line1.right}
    Text { id : line2; text : qsTr("Raster Size"); width: 120; font.bold: true ;anchors.top : line1.bottom; height : lineheight}
    Text { id : dimText; text : dimensions;  height : lineheight; anchors.left: line1.right;anchors.top : line1.bottom}
    Text { text : "; pixelsize = " + getProperty("pixelsize");  height : lineheight; anchors.left: dimText.right;anchors.top : line1.bottom}

}
