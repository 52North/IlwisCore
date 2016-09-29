import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0
import UIContextModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Item {
    width : 200

    anchors.topMargin: 3
    property string iconName : "../images/start"

    Rectangle {
        id : header
        width : 620 * Global.uiScale  -2
        height : 18
        color : Global.palegreen
        x : 2
        Text{
            id :lbl1
            text : qsTr("Run python statements")
            font.bold: true
            x : 3
            width : 198
            anchors.verticalCenter: parent.verticalCenter
        }
        Text{
            id : lbl2
            text : qsTr("Content management")
            font.bold: true
            width : 198
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: lbl1.right
            anchors.leftMargin: 12
        }
        Text{
            text : qsTr("File Management")
            font.bold: true
            width : 198
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: lbl2.right
            anchors.leftMargin: 10
        }

    }
    Column {
        width : 200 * Global.uiScale
        height : parent.height - header.height
        anchors.top : header.bottom
        anchors.topMargin: 4
        y: 5 * Global.uiScale
        x : 5 * Global.uiScale
        id : buttonRow
        spacing : 8
        Controls.WideButton{
            image : "../images/rundelete.png"
            label : qsTr("Run")
            width : buttonRow.width
            height : 40 * Global.uiScale

            onClicked: {
                scriptArea.runDelete()
            }
            Controls.ToolTip{
                target : parent
                text : qsTr("Runs all the statement in the console area and then moves them to the history")
            }
        }
        Controls.WideButton{
            image : "../images/runkeep.png"
            label : qsTr("Run & Keep")
            width : buttonRow.width
            height : 40 * Global.uiScale

            onClicked: {
                scriptArea.runKeep()
            }
            Controls.ToolTip{
                target : parent
                text : qsTr("Runs all the statement in the console area but keeps the text in the console")
            }
        }
        Controls.WideButton{
            image : "../images/runselected.png"
            label : qsTr("Run Selected")
            width : buttonRow.width
            height : 40 * Global.uiScale

            onClicked: {
                scriptArea.runSelected()
            }
            Controls.ToolTip{
                target : parent
                text : qsTr("Runs the selected statement in the console")
            }
        }
    }
    Column {
        width : 200 * Global.uiScale
        height : parent.height - header.height
        anchors.top : header.bottom
        anchors.topMargin: 4
        anchors.left: buttonRow.right
        anchors.leftMargin: 8
        y: 5 * Global.uiScale
        x : 5 * Global.uiScale
        id : buttonRow2
        spacing : 8

        Controls.WideButton{
            image : "../images/deleteC.png"
            label : qsTr("Clear console")
            width : buttonRow.width
            height : 40 * Global.uiScale

            onClicked: {
                scriptArea.clear('c')
            }
            Controls.ToolTip{
                target : parent
                text : qsTr("clears all the text from the console")
            }
        }
        Controls.WideButton{
            image : "../images/deleteH.png"
            label : qsTr("Clear History")
            width : buttonRow.width
            height : 40 * Global.uiScale

            onClicked: {
                  scriptArea.clear('h')
            }
            Controls.ToolTip{
                target : parent
                text : qsTr("clears all the text from the history")
            }
        }
        Controls.WideButton{
            image : "../images/deleteO.png"
            label : qsTr("Clear results")
            width : buttonRow.width
            height : 40 * Global.uiScale

            onClicked: {
                  scriptArea.clear('o')
            }
            Controls.ToolTip{
                target : parent
                text : qsTr("Clears all the content from the results")
            }
        }
    }
    Column {
        width : 200 * Global.uiScale
        height : parent.height - header.height
        anchors.top : header.bottom
        anchors.topMargin: 4
        anchors.left: buttonRow2.right
        anchors.leftMargin: 8
        y: 5 * Global.uiScale
        x : 5 * Global.uiScale
        id : buttonRow3
        spacing : 8

        Controls.WideButton{
            image : "../images/save.png"
            label : qsTr("Save")
            width : buttonRow.width
            height : 40 * Global.uiScale
            enabled : tabmodel && tabmodel.displayName === "Python Console" ? false : true
            opacity : enabled ? 1.0 : 0.5

            onClicked: {

            }
            Controls.ToolTip{
                target : parent
                text : qsTr("saves the script under its current name")
            }
        }
        Controls.WideButton{
            image : "../images/saveas.png"
            label : qsTr("Save as...")
            width : buttonRow.width
            height : 40 * Global.uiScale

            onClicked: {

            }
            Controls.ToolTip{
                target : parent
                text : qsTr("saves the script under a new name")
            }
        }
    }
}
