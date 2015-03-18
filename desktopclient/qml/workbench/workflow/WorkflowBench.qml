import QtQuick 2.0

import QtQuick.Controls 1.0

import WorkflowModel 1.0

import ".." as Workbench

Rectangle {

    id : workflowbench

    anchors.bottom: parent.bottom

    // ###########################  TITLE

    Rectangle {
        id: title
        width: parent.width
        height: 24
        anchors.margins: 15

        Text {
            text: qsTr("Workflow Bench")
            font.family: "Arial"
            font.bold: true
            font.pixelSize: 16
        }

        WorkflowSeparator { }
    }



    // ###########################  CONTROL BUTTONS

    Rectangle {
        id: workflowBenchButtons
        anchors.top : title.bottom
        anchors.margins: 5
        width: parent.width
        height : 24
        x : parent.x

        Button {
            id : newWorkflow
            text :  qsTr("New Workflow")
            anchors.margins: 5
            onClicked : console.log("TODO new workflow")
            enabled: true
        }

        WorkflowSeparator { }
    }


    // ###########################  Workflow Metadata

    function createWorkflowMetadataForm(metaid, title){
        var form = formbuilder.createWorkflowForm(metaid)
        appFrame.formQML = form
        appFrame.formTitle = title
        appFrame.opacity = 1
    }

    SplitView {
        x : parent.x
        anchors.top : workflowBenchButtons.bottom
        anchors.bottom: workflowbench.bottom
        anchors.margins: 5
        width : workflowbench.width
        orientation: Qt.Vertical
        resizing: false

        Rectangle {
            id : applicationForm
            height : 0
            x : parent.x + 5

            Workbench.ApplicationForm {
                id : appFrame
                width : parent.width
                height : parent.height - 30 < 0 ?  0 : parent.height - 30
                opacity : 0

                // ###########################  CONTROL BUTTONS

                Rectangle {
                    id: workflowButtons
                    anchors.top : appFrame.bottom
                    anchors.margins: 5
                    width: parent.width
                    height : 24
                    x : parent.x

                    Button {
                        id : openWorkflowButton
                        text :  qsTr("Open Workflow")
                        x : parent.x
                        anchors.margins: 5
                        onClicked: console.log("TODO open workflow")
                        enabled: true
                    }
                    Button {
                        id : deleteButton
                        text :  qsTr("Delete")
                        anchors.left: openWorkflowButton.right
                        anchors.margins: 5
                        onClicked: console.log("TODO delete workflow and close appFrame")
                        enabled: true
                    }
                    Button {
                        id : cancelButton
                        text :  qsTr("Cancel")
                        anchors.left: deleteButton.right
                        anchors.margins: 5
                        onClicked: console.log("TODO close appFrame")
                        enabled: true
                    }
                }

            }

            /* TODO make this an + model item which is editable (let user customize metadata)

            Text {
                id : workflowNameLabel
                height : 20
                width: 50
                text : qsTr("Name")
                verticalAlignment: Text.AlignVCenter
                clip: false
                y : 4
            }
            TextField {
                id : workflowNameText
                y: 4
                height: 20
                anchors.leftMargin: 5
                anchors.left: workflowNameLabel.right
                anchors.right: parent.right
                onFocusChanged: {
                    console.log("workflow name: " + workflowNameText.text)
                }
            }
            */
            states: [
                State {
                    name: "maximized"
                    PropertyChanges {
                        target: applicationForm
                        height : 200
                        opacity : 1

                    }
                },
                State {
                    name : "minimized"
                    PropertyChanges {
                        target: applicationForm
                        height : 0
                        opacity : 0
                    }
                }

            ]
            transitions: [
                Transition {
                    NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
                    NumberAnimation { properties: "opacity"; duration : 750 ; easing.type: Easing.InOutCubic }
                }
            ]
        }

        // ###########################  FILTER

        Rectangle {
            id : workflowFilterBox
            x : parent.x
            width : parent.width
            height : 28
            anchors.margins : 5
            anchors.top : applicationForm.bottom
            color : background2

            Text {
                id : searchTextLabel
                height : 20
                width: 50
                text : qsTr("Filter Text")
                verticalAlignment: Text.AlignVCenter
                clip: false
                y : 4
            }
            TextField {
                id : searchText
                y: 4
                height: 20
                anchors.leftMargin: 5
                anchors.left: searchTextLabel.right
                anchors.right: parent.right
                onTextChanged: {
                    workflows.nameFilter = text
                }
            }
        }



        // ###########################  WORKFLOWS
        /*
        WorkflowList {
            id: workflows
            Connections {
                target : workflowsList
            }
        }
        */

        // ###########################  OPERATIONS (until workflows complete)



        TabView {
            id : workflows
            anchors.bottom: parent.bottom
            width : workflowbench.width
            Tab {
                title : qsTr("Workflow/Operation List")
                WorkflowList {
                    id : workflowList
                    Connections {
                        target : workflowList

                    }
                }
            }
        }
    }

}
