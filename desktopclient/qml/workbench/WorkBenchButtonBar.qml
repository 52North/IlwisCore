import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtGraphicalEffects 1.0
import MasterCatalogModel 1.0
import TranquilizerHandler 1.0
import UIContextModel 1.0
import "../Global.js" as Global
import "../controls" as Controls

Rectangle {
    id : buttonB
    y : 0
    height : parent.height
    width :colContainer.width
    color : uicontext.paleColor

    function setButtonStatus(buttonname, status) {
        if ( status === "toggle"){
            toggle(buttonname,status)
            return
        }

        prop.checked = nav.checked = oper.checked = errors.checked = progress.checked = create.checked = preferences.checked = false
        if ( buttonname === "navigator"){
            nav.checked = status === "on"
        }
        if ( buttonname === "operationselection")
            oper.checked = status === "on"
        if ( buttonname === "objectproperties")
            prop.checked = status === "on"
        if ( buttonname === "messages")
            errors.checked = status === "on"
        if ( buttonname === "tranquilizer")
            progress.checked = status === "on"
        if ( buttonname === "objectcreation")
            create.checked = status === "on"
        if ( buttonname === "info")
            info.checked = status === "on"
        if ( buttonname === "preferences")
            preferences.checked = status === "on"

    }
    function toggle(button, visibility) {
        var currentValue = button.checked
        prop.checked = nav.checked = oper.checked = errors.checked = progress.checked = create.checked = preferences.checked = false
        button.checked = !currentValue
        butColumn.initial = false
    }

    DropShadow {
        id: butShadow
       // z: colContainer.z + 1
        anchors.fill: source
        cached: true
        horizontalOffset: 4
        radius: 6
        samples: 12
        color: "#80000000"
        smooth: true
        opacity: 0.7
        source: colContainer
    }

    Item {
        id : colContainer
        width : butColumn.width + 7
        height : parent.height
        Column {
            id : butColumn
            width : 70
            height : Math.min(1100,parent.height)
            property bool initial : true
            z: buttonB.z + 1

            WorkBenchButton{
                id : nav
                objectName : "workbench_navbutton_mainui"
                iconname: butColumn.initial ? "locator_start.png" : "locator.png"
                label: qsTr("Locator")

                function mouseClicked() {
                    toggle(nav)
                    transitionInfoPane("navigator","toggle")
                }

                Controls.ToolTip {
                    target: nav
                    text: qsTr("Access to navigation over data sources. Bookmark creation")
                }
            }

            WorkBenchButton{
                id : oper
                iconname : "operations.png"
                label: qsTr("Operations")

                function mouseClicked() {
                    toggle(oper)
                    transitionInfoPane("operationselection","toggle")
                }

                Controls.ToolTip {
                    target: oper
                    text: qsTr("Browsing and executing data operations")
                }

            }
            WorkBenchButton {
                id : consoleBut
                iconname : "console.png"
                label: qsTr("Python")
                checkable: false

                function mouseClicked() {
                    var id = uicontext.consoleScriptId()
                    bigthing.newCatalog("itemid=" + id, "script", "ilwis://internalcatalog/consolescript.py", "other")
                }

                Controls.ToolTip {
                    target: prop
                    text: qsTr("Opens the Python console to enter python script commands")
                }
            }

            WorkBenchButton{
                id : prop
                iconname : "metadata.png"
                label: qsTr("Metadata")

                function mouseClicked() {
                    toggle(prop)
                    transitionInfoPane("objectproperties","toggle")
                }

                Controls.ToolTip {
                    target: prop
                    text: qsTr("Acces to metadata of IlwisObjects selected in the active catalog")
                }

            }
            WorkBenchButton{
                id : create
                iconname : "create.png"
                label: qsTr("Create")

                function mouseClicked() {
                    toggle(create)
                    transitionInfoPane("objectcreation","toggle")
                }

                Controls.ToolTip {
                    target: create
                    text: qsTr("Creation of IlwisObjects")
                }
            }
            WorkBenchButton{
                id : errors
                iconname : messagehandler.messageIcon
                label: qsTr("Messages")

                function mouseClicked() {
                    toggle(errors)
                    transitionInfoPane("messages","toggle")
                }

                Controls.ToolTip {
                    target: errors
                    text: qsTr("Messages generated by the system including warnings and errors")
                }
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
                    opacity: (value > 0) ? 0.55 : 0

                    style: ProgressBarStyle {
                        background: Rectangle {
                            radius: 2
                            color: "grey"
                            border.color: "gray"
                            border.width: 1
                        }
                        progress: Rectangle {
                            color: uicontext.lightColor
                            border.color: uicontext.darkestColor
                        }
                    }

                }

                iconname : "progress.png"
                label: qsTr("Progress")

                function mouseClicked() {
                    toggle(progress)
                    transitionInfoPane("tranquilizers","toggle")
                }

                Controls.ToolTip {
                    target: progress
                    text: qsTr("Progress of currently running operations and actions")
                }

            }
            WorkBenchButton{
                id : preferences
                iconname : "settings.png"
                label: qsTr("Settings")

                function mouseClicked() {
                    toggle(preferences)
                    transitionInfoPane("preferences","toggle")
                }

                Controls.ToolTip {
                    target: preferences
                    text: qsTr("User defined settings for Ilwis 4")
                }

            }
            WorkBenchButton{
                id : info
                iconname : "info.png"
                label: qsTr("Info")

                function mouseClicked() {
                    toggle(info)
                    transitionInfoPane("info","toggle")
                }

                Controls.ToolTip {
                    target: info
                    text: qsTr("Information on Ilwis 4")
                }
            }

        }
        Rectangle {
            anchors.top : butColumn.bottom
            anchors.bottom: colContainer.bottom
            width : parent.width - 7
            color : uicontext.middleColor
        }
    }

    states: [
        State { name: "visible"

            PropertyChanges {
                target: buttonB
                width : 70
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
