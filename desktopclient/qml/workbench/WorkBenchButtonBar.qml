import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0


Rectangle {
    id : buttonB
    y : 0
    height : parent.height
    width : 80
    color : "white"
    border.width : 1
    Layout.maximumWidth : 140

    //signal transitionInfoPane(string pagename)

    Action {
        id : navClicked
        onTriggered : {
            transitionInfoPane("Navigator.qml")
        }

    }

    Action {
        id : operClicked
        onTriggered : {
            transitionInfoPane("OperationSelection.qml")
        }

    }
    Action {
        id : propertiesClicked
        onTriggered : {
            transitionInfoPane("ObjectProperties.qml")
        }
    }
    Action {
        id : messagesClicked
        onTriggered : {
            transitionInfoPane("MessagesPane.qml")
        }
    }
    Action {
        id : progressClicked
        onTriggered : {
            transitionInfoPane("Tranquilizers.qml")
        }

    }
    Action {
        id : modellerClicked
        onTriggered : {
            transitionInfoPane("modeller/ModellerSelection.qml")
        }

    }
    Action {
        id : workspaceClicked
        onTriggered : {
            transitionInfoPane("Workspaces.qml")
        }

    }

    Action {
        id :maxButtons
        onTriggered: {
            buttonB.width = 55
        }
    }

    Action {
        id :minButtons
        onTriggered: {
            buttonB.width = 0
        }
    }

    Column {
        anchors.fill: parent
        Rectangle {
            height : 21
            width : buttonB.width
            color : background4
            Row {
                anchors.fill: parent
                Button{
                    id : full
                    height : 20
                    width :20
                    action : maxButtons
                    Image { anchors.centerIn : parent; source: "../images/max1.png" }
                }
                Button{
                    id : close
                    height : 20
                    width :20
                    action : minButtons
                    Image { anchors.centerIn : parent; source: "../images/min1.png" }
                }
            }
        }

        WorkBenchButton{
            id : nav
            action: navClicked
            iconname: "navigatorCS1.png"
            label: qsTr("Navigator")
        }

        WorkBenchButton{
            id : oper
            action: workspaceClicked
            iconname : "workspaceCS1.png"
            label: qsTr("Workspace")
        }
        WorkBenchButton{
            id : workspace
            action: operClicked
            iconname : "operationCS1.png"
            label: qsTr("Operations")
        }
        WorkBenchButton{
            id : modeller
            action: modellerClicked
            iconname : "modellerCS1.png"
            label: qsTr("Modeller")
        }
        WorkBenchButton{
            id : prop
            action : propertiesClicked
            iconname : "propertiesCS1.png"
            label: qsTr("Properties")

        }
        WorkBenchButton{
            id : errors
            action : messagesClicked
            iconname : messagehandler.messageIcon
            label: qsTr("Messages")
        }
        WorkBenchButton{
            id : progress
            action : progressClicked
            iconname : "progressCS1.png"
            label: qsTr("Progress")

        }
        WorkBenchButton{
            id : preferences
            //action : progressClicked
            iconname : "preferencesCS1.png"
            label: qsTr("Preferences")

        }
    }
}
