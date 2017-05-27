import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Rectangle {
    id : container

    function storeData() {
    }

    height : 70
    property string domainUsed
    property string valueTypeUsed
    property string domainTypeUsed
    property string columnName
    //color : index  % 2 == 0 ? "#F7F9FC" : "#DCDCDC"
    color : uicontext.lightestColor

    function rangeFieldType(rdef) {
        var index = rdef.indexOf(":")
        var rtype = rdef.substring(0,index)
        if ( rtype === "thematicrange" || rtype === "namedidentifierrange"  ||
                rtype === "colorpalette" ||  rtype === "intervalrange"){
            return "RangeValuesCombo.qml"

        }
        if (rtype === "indexedidentifierrange"){
            return "IndexItemRange.qml"
        }

        return "TextRangeDefinition.qml";
    }

    Text { id : line1; text : qsTr("Domain"); width: 100; font.italic: true }
    Text { text : domainUsed == "" ? qsTr("not applicable") : domainUsed;  height : 20;width: parent.width - line1.width - 2; anchors.left: line1.right}
    Text { id : line2; text : qsTr("Value type"); width: 100; font.italic: true; anchors.top : line1.bottom }
    Text { text : valueTypeUsed; width: parent.width - line2.width - 2; anchors.left: line2.right; anchors.top : line1.bottom}
    Text { id : line3; text : qsTr("Default range"); width: 100; font.italic: true; anchors.top : line2.bottom}
    Loader { id: defaultRangeLoader
        width: parent.width - line3.width - 2
        anchors.left: line3.right
        anchors.top : line2.bottom
        height: 20
        Component.onCompleted:{ defaultRangeLoader.setSource(rangeFieldType(rangeDefinition(true, false, columnName)),{"useDefaultRange":true})}
    }
    Loader {
         id : actualLoader
         anchors.top : line3.bottom
         width : parent.width
         source : domainTypeUsed == "ValueDomain" ? "ActualNumericRange.qml" : ""
    }
}

