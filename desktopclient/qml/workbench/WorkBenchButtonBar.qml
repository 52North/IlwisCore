import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import TranquilizerHandler 1.0
import UIContextModel 1.0
import "../Global.js" as Global

Rectangle {
    id : buttonB
    y : 0
    height : parent.height
    width : 80
    color : Global.alternatecolor2
    Layout.maximumWidth : 140

    //signal transitionInfoPane(string pagename)

    function toggle(button) {
        var currentValue = button.checked
        prop.checked = nav.checked = workspace.checked = oper.checked = errors.checked = progress.checked = create.checked = preferences.checked = workspace.checked = false
        button.checked = !currentValue
    }

        Action {
            id : navClicked
            onTriggered : {
               toggle(nav)
                transitionInfoPane("navigator")
            }

        }

        Action {
            id : operClicked
            onTriggered : {
                toggle(oper)
                transitionInfoPane("operationselection")
            }

        }
        Action {
            id : propertiesClicked
            onTriggered : {
                toggle(prop)
                transitionInfoPane("objectproperties")
            }
        }
        Action {
            id : messagesClicked
            onTriggered : {
                toggle(errors)
                transitionInfoPane("messages")
            }
        }
        Action {
            id : progressClicked
            onTriggered : {
                toggle(progress)
                transitionInfoPane("tranquilizer")
            }

        }
        Action {
            id : infoClicked
            onTriggered : {
                transitionInfoPane("info")
            }

        }
        Action {
            id : createClicked
            onTriggered : {
                toggle(create)
                transitionInfoPane("objectcreation")
            }

        }
        Action {
            id : workspaceClicked
            onTriggered : {
                toggle(workspace)
                transitionInfoPane("workspaces")
            }

        }

        Action {
            id : prefClicked
            onTriggered : {
                toggle(preferences)
                mastercatalog.longAction()
            }

        }

    Column {
        anchors.fill: parent

        WorkBenchButton{
            id : nav
            objectName : "workbench_navbutton_mainui"
            action: navClicked
            iconname: "navigatorCS1.png"
            label: qsTr("Navigator")
        }

        WorkBenchButton{
            id : workspace
            action: workspaceClicked
            iconname : "workspaceCS1.png"
            label: qsTr("Workspace")
        }
        WorkBenchButton{
            id : oper
            action: operClicked
            iconname : "operationCS1.png"
            label: qsTr("Operations")
        }

        WorkBenchButton{
            id : prop
            action : propertiesClicked
            iconname : "propertiesCS1.png"
            label: qsTr("Metadata")

        }
        WorkBenchButton{
            id : create
            action: createClicked
            iconname : "createCS.png"
            label: qsTr("Create")
        }
        WorkBenchButton{
            id : errors
            action : messagesClicked
            iconname : messagehandler.messageIcon
            label: qsTr("Messages")
        }
        WorkBenchButton{
            id : progress
            ProgressBar {
                anchors.top: progress.top
                anchors.topMargin: 5
                width : parent.width - 10
                x : 5
                height : 12
                maximumValue: 100
                minimumValue: 0
                value : tranquilizerHandler.aggregateValue
                opacity: value > 0 ? 0.35 : 0
                style: ProgressBarStyle {
                     background: Rectangle {
                         radius: 2
                         color: "lightgreen"
                         border.color: "gray"
                         border.width: 1
                     }
                }

            }

            action : progressClicked
            iconname : "progress2CS1.png"
            label: qsTr("Progress")

        }
        WorkBenchButton{
            id : preferences
            action : prefClicked
            iconname : "preferencesCS1.png"
            label: qsTr("Preferences")

        }
        WorkBenchButton{
            id : info
            action: infoClicked
            iconname : "helpCS1.png"
            label: qsTr("Info")
        }

    }
    states: [
        State { name: "visible"

            PropertyChanges {
                target: buttonB
                width : 80
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: buttonB
                width : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "width"; duration : 500 ; easing.type: Easing.InOutCubic }
        }
    ]
}
