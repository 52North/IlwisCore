import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Rectangle {
    function storeData() {
        if ( propertyForm.editable){

        }
    }
    color : uicontext.lightestColor

    Column {
        id : texts
        x: 4
        y : 10
        width : parent.width
        height : Global.rowHeight * 4
        spacing: 3
        Controls.TextEditLabelPair{ labelText:   qsTr("X Axis Domain"); content : getProperty("xaxisdomain"); labelWidth: 120;width : parent.width; readOnly: true}
        Controls.TextEditLabelPair{ labelText : qsTr("Y Axis Domain"); content : getProperty("yaxisdomain"); labelWidth: 120;width : parent.width; readOnly: true}
        Controls.TextEditLabelPair{ labelText : qsTr("Combination domain"); content : getProperty("combodomain"); labelWidth: 120;width : parent.width; readOnly: true}


    }

    GridView {
        anchors.top : texts.bottom
        anchors.topMargin: 4
        id : grid
        width : 120* (parseInt(getProperty("xaxisvalueslength")) + 1)
        height: 20* (parseInt(getProperty("yaxisvalueslength")) + 1)
        cellWidth: 120
        cellHeight: 20
        model : getProperty("combinationmatrix").split("|")
        delegate: Rectangle {
            width : 120
            height : 20
            border.width: 1
            border.color: Global.edgecolor
            color : index == 0 ? uicontext.lightColor :  "white"
            Text{
                x: 2
                y : 2
                height : 18
                width : parent.width - 3
                text : modelData
                clip : true
                elide : Text.ElideMiddle
            }
        }

    }

//    Text { id : line1; text : qsTr("X Axis Domain"); width: 100; font.italic: true }
//    Text { text : getProperty("xaxisdomain");  height : 20;width: parent.width - line1.width - 2; anchors.left: line1.right}
//    Text { id : line2; text : qsTr("Y Axis Domain"); width: 100; font.italic: true; anchors.top : line1.bottom }
//    Text { text : getProperty("yaxisdomain"); width: parent.width - line2.width - 2; anchors.left: line2.right; anchors.top : line1.bottom}
//    Text { id : line3; text : qsTr("Combination domain"); width: 100; font.italic: true; anchors.top : line2.bottom }
//    Text { text : getProperty("combodomain"); width: parent.width - line3.width - 2; anchors.left: line3.right; anchors.top : line2.bottom}


}
