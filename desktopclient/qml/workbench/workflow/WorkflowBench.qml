import QtQuick 2.0
import QtQuick.Controls 1.0

import WorkflowMetadataFormBuilder 1.0
import WorkflowCatalogModel 1.0

import ".." as Workbench

import "../../datapanel/workflow" as WorkflowDataTest
import "../../datapanel" as DataPane

Rectangle {

    id : workflowbench


    // ###########################  TITLE

    WorkflowBenchHeader {
        id: workflowbenchtitle
        titleText: qsTr("Workflow Bench")
        shortDescriptionText: qsTr("To edit an workflow, select an existing or create a new one.")
    }


    // ###########################  CONTROL BUTTONS

    Action {
        id: createNewWorkflow
        onTriggered: {
            var workflow = workflows.newWorkflow("workflow");
            createWorkflowMetadataForm(workflow.id, workflow.displayName);
        }
    }

    Rectangle {
        id: workflowBenchButtons
        anchors.top : workflowbenchtitle.bottom
        anchors.margins: 5
        width: parent.width
        height : 24
        x : parent.x

        Button {
            id : newWorkflow
            anchors.margins: 5
            action : createNewWorkflow
            text :  qsTr("New Workflow")
            enabled: true
        }

        WorkflowSeparator { }
    }


    // ###########################  Workflow Metadata

    function createWorkflowMetadataForm(metaid, title) {
        //var form = formbuilder.index2Form(metaid);
        var form = workflowmetadataformbuilder.createWorkflowForm(metaid)
        appFrame.formQML = form;
        appFrame.formTitle = title;
        appFrame.opacity = 1
        applicationForm.state = applicationForm.state != "minimized" ? "minimized" : "maximized"
    }

    function createWorkflowEditSession(workflowName) {
        dataPanel.addWorkflowCanvas(workflowName + " [Workflow Builder]" );
        workflowbenchContentLoader.setSource("WorkflowEdit.qml");

        // GUI state is not available right now --> use model instead
        workflows.currentWorkflow = workflowName
    }

    SplitView {
        width : parent.width
        orientation: Qt.Vertical
        anchors.bottom: parent.bottom
        anchors.top : workflowBenchButtons.bottom
        anchors.margins: 5
        resizing: false

        Rectangle {
            id : applicationForm
            anchors.top : parent.top
            x : parent.x + 5
            height : 0

            property string workflowname
            property string workflowdescription

            // ###########################  CONTROL BUTTONS

            Rectangle {
                id: workflowButtons
                anchors.top : parent.top
                anchors.margins: 5
                height : 20

                Button {
                    id : editWorkflowButton
                    text :  qsTr("Edit Workflow")
                    onClicked: createWorkflowEditSession(applicationForm.workflowname)
                    tooltip: qsTr("Edit selected workflow")
                    anchors.margins: 5
                }
                Button {
                    id : saveButton
                    text :  qsTr("Save Metadata")
                    anchors.left: deleteButton.right
                    onClicked: console.log("TODO save metadata")
                    tooltip: qsTr("Saves workflow's metadata. TODO becomes enabled when something changes")
                    anchors.margins: 5
                    enabled: false
                }
                Button {
                    id : deleteButton
                    text :  qsTr("Delete")
                    anchors.left: editWorkflowButton.right
                    tooltip: qsTr("TODO delete workflow and close appFrame")
                    anchors.margins: 5
                    enabled: true
                }

                WorkflowSeparator {}
            }


            // ###########################  DYNAMIC I/O FORM

            Workbench.ApplicationForm {
                id : appFrame
                width : parent.width
                height : parent.height - 30 < 0 ?  0 : parent.height - 30
                anchors.top : workflowButtons.bottom;
                opacity: 0
            }

            Button{
                y : parent.height - 25
                text : "Create Workflow"
                height : 22
                x : parent.width - width
                onClicked: {
                    workflows.newWorkflow(applicationForm.workflowname,
                                          applicationForm.workflowdescription)
                    createWorkflowEditSession(applicationForm.workflowname);
                }
            }
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

            WorkflowSeparator {}
        }


        // ###########################  FILTER

        FilterTextField {
            id: filterBox
            anchors.top : applicationForm.bottom
            Connections {
                target: filterBox
                onFilterChanged : {
                    workflows.nameFilter = text
                }
            }
        }


        // ###########################  WORKFLOWS

        TabView {
            id : workflowView

            Tab {
                title : qsTr("Workflow/Operation List")
                WorkflowList {
                    id : workflowList

                }
            }
        }
    }

}
