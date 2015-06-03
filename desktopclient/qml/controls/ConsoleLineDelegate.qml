import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import ConsoleLineModel 1.0
import "../Global.js" as Global

Item {
    id : lineDelegate
    width: parent.width
    height: 25
    property string consoletext : content
    property string line : lineNumber
    property int listindex : index
    property bool asMainEdit : false
    property bool editFocus : false

    onEditFocusChanged: {
        inputField.focus = editFocus
    }

    signal runLine(string txt, int lineIndex)

    Row {
        width : parent.width
        height : parent.height
        Rectangle{
            id : linenum
            color : Global.alternatecolor5
            height : parent.height
            width : 30
            Text {
                text : line

                x : 3
                y : 4
            }
        }
        Rectangle{
            id : marker
            width : 24
            height : parent.height
            color : Global.alternatecolor3
            Text {

                text : ">>>"
                y: 4
            }
        }

        TextField {
            id : inputField
            text : consoletext
            anchors.verticalCenter: parent.Center
            width : parent.width - marker.width - linenum.width
            height : parent.height+ 1
            activeFocusOnTab : true
            style: TextFieldStyle {
                 textColor: "black"
                 background: Rectangle {
                    width : inputField.width
                    height : inputField.height
                    color : listindex == consolearea.currentIndex ? Global.alternatecolor3 : Global.alternatecolor2
                    border.width: 1
                    border.color: asMainEdit ? "grey" : "#eeeeee"
                 }
            }
            onAccepted: {
                runLine(text, asMainEdit ? -1 : listindex)
            }
        }
    }
}

