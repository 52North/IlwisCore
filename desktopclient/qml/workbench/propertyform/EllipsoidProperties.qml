import QtQuick 2.0
import "../../Global.js" as Global

Rectangle {
    function storeData() {
        if ( propertyForm.editable){
        }
    }
       color : uicontext.lightestColor

    Text { id : line1; text : qsTr("Name"); width: 100; font.italic: true }
    Text { text : displayName;  height : 20;width: parent.width - line1.width - 2; anchors.left: line1.right}
    Text { id : line2; text : qsTr("Major axis"); width: 100; font.italic: true; anchors.top : line1.bottom }
    Text { text : getProperty("majoraxis"); width: parent.width - line2.width - 2; anchors.left: line2.right; anchors.top : line1.bottom}
    Text { id : line3; text : qsTr("Minor axis"); width: 100; font.italic: true; anchors.top : line2.bottom }
    Text { text : getProperty("minoraxis"); width: parent.width - line3.width - 2; anchors.left: line3.right; anchors.top : line2.bottom}
    Text { id : line4; text : qsTr("Flattening"); width: 100; font.italic: true; anchors.top : line3.bottom }
    Text { text : getProperty("flattening"); width: parent.width - line4.width - 2; anchors.left: line4.right; anchors.top : line3.bottom}
    Text { id : line5; text : qsTr("Excentricity"); width: 100; font.italic: true; anchors.top : line4.bottom }
    Text { text : getProperty("excentricity"); width: parent.width - line5.width - 2; anchors.left: line5.right; anchors.top : line4.bottom}
}

