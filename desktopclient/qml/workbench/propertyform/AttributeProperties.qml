import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Rectangle {
    id : container


    height : 70
    property string domainUsed
    property string valueTypeUsed
    color : index  % 2 == 0 ? "#F7F9FC" : "#DCDCDC"

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
    function storeData() {
        if ( propertyForm.editable){
            setAttribute("domain", domainField.getText())
        }
    }

    Text { id : line1; text : qsTr("Domain"); width: 96; height : Global.rowHeight; font.italic: true }
    TextFieldDropArea { id: domainField; currentText: domainUsed == "" ? qsTr("not applicable") : domainUsed;  height : Global.rowHeight;width: 200; anchors.left: line1.right}
    Text { id : line2; text : qsTr("Value type"); width: 100; height : Global.rowHeight; font.italic: true; anchors.top : line1.bottom }
    Text { text : valueTypeUsed; width: parent.width - line2.width - 2; height : Global.rowHeight; anchors.left: line2.right; anchors.top : line1.bottom}
    Text { id : line3; text : qsTr("Default range"); width: 100; height : Global.rowHeight; font.italic: true; anchors.top : line2.bottom}
    Loader { id: defaultRangeLoader
        width: parent.width - line3.width - 2
        anchors.left: line3.right
        anchors.top : line2.bottom
        height:Global.rowHeight
        Component.onCompleted:{ defaultRangeLoader.setSource(rangeFieldType(rangeDefinition(true,false)),{"useDefaultRange":true})}
    }
    Text { id : line4; text : qsTr("Actual range"); width: 100; height : Global.rowHeight; font.italic: true; anchors.top : line3.bottom}
    Loader { id: actualRangeLoader
        width: 164
        anchors.left: line4.right
        anchors.top : line3.bottom
        height:Global.rowHeight
        Component.onCompleted:{ actualRangeLoader.setSource(rangeFieldType(rangeDefinition(false,false)),{"useDefaultRange":false})}
    }
    Button {
        anchors.left: actualRangeLoader.right
        anchors.leftMargin: 4
        anchors.top : line3.bottom
        width : 60
        height : 20
        text : qsTr("Calculate")
        onClicked: {
            actualRangeLoader.source = ""
            actualRangeLoader.setSource(rangeFieldType(rangeDefinition(false,true)),{"useDefaultRange":false})
        }
    }
}

