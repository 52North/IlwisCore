import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../Global.js" as Global

Rectangle {
    id : container
    anchors.fill : parent
    color : uicontext.lightestColor
    anchors.margins: 2

    function rangeFieldType(rdef) {
        var index = rdef.indexOf(":")
        var rtype = rdef.substring(0,index)
        if ( rtype === "thematicrange" || rtype === "namedidentifierrange"  ||
                rtype === "indexedidentifierrange" || rtype === "colorpalette" ||
                rtype === "intervalrange"){
            return "RangeValuesCombo.qml"

        }
        return "TextRangeDefinition.qml";
    }

    GridLayout{
        id : grid
        height : 80
        width : parent.width
        columns : 2
        Text { text : qsTr("Domain"); Layout.minimumWidth: 100; font.bold: true }
        Text { text : domainName; Layout.minimumWidth: parent.width - 100 - 2; }
        Text { text :  qsTr("Value type xxx"); font.bold: true }
        Text { text : valuetype}
        Text { text : qsTr("Default range"); Layout.minimumWidth: 100; font.bold: true}
        Loader { width : 100 ; height: 22 ;source: rangeFieldType(rangeDefinition)}

    }

}

