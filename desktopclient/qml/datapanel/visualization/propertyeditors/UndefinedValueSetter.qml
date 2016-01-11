import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../../controls" as Controls

Column {
    width : 200
    height : 60
    spacing : 4
    property var editor
    Controls.TextEditLabelPair{
        id : valueedit
        width : 300
        height : 20
        labelText: qsTr("Pseudo undefined value")
        labelWidth: 140
        regexvalidator: /^\d*(\.\d*)?$/

    }
    Button {
        width : 80
        height : 20
        text : qsTr("Apply")
        anchors.right: valueedit.right
        anchors.rightMargin: 3
        onClicked: {
            if ( valueedit.content != ""){
                var result  = parseFloat(valueedit.content)
                if (  !isNaN(result)){
                    editor.undefinedvalue = result
                }
            }
        }
    }
}

