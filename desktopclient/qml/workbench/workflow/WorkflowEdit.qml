import QtQuick 2.0
import QtQuick.Controls 1.0

import ApplicationFormExpressionParser 1.0

import ".." as Workbench
import "../.." as Base

Rectangle {

    id : workflowedit

    function newForm(metaid, title, url){
        var form = formbuilder.index2Form(metaid)
        appFrame.formQML = form
        appFrame.formTitle = title
        appFrame.operationId = metaid
        appFrame.operationUrl = url
        appFrame.opacity = 1
    }


    // ###########################  TITLE

    WorkflowBenchHeader {
        id: workflowedittitle
        titleText: qsTr("Edit Workflow")
    }

    // ###########################  CONTROL BUTTONS

    Action {
        id: cancelEditing
        onTriggered: {
            console.log("Edit cancelled by user")

            // TODO restore selection status when getting back?
            dataPanel.removeWorkflowCanvas(workflowbenchContentLoader.editSession + " [Workflow Builder]")

            workflowbenchContentLoader.setSource("WorkflowBench.qml")
        }
    }


    Rectangle {
        id: workflowEditButtons
        anchors.top : workflowedittitle.bottom
        anchors.margins: 5
        width: parent.width
        height : 24
        x : parent.x

        Button {
            id : saveWorkflow
            text :  qsTr("Save Workflow")
            anchors.margins: 5
            onClicked : console.log("TODO save workflow")
            enabled: true
        }

        Button {
            id : cancel
            text :  qsTr("Cancel Editing")
            anchors.margins: 5
            anchors.left: saveWorkflow.right
            action : cancelEditing
            enabled: true
        }

        WorkflowSeparator { }
    }

    // ###########################  WORKFLOW'S IO SECTION

    // To the framwork a workflow shall appear just as an operation;
    // An operation provides OperationMetadata from where dynamic
    // IO form is being created. Here the same form can be used, but
    // would not have to be hidden by default. Displaying workflow's
    // IO parmaeters or an empty element with a description of next
    // steps (see sketch) would be ok. The form has to be updated,
    // though depending on canvas selection signals, e.g. operation
    // gets selected

    SplitView {
        width : parent.width
        anchors.bottom: parent.bottom
        anchors.top : workflowEditButtons.bottom;
        orientation: Qt.Vertical
        anchors.topMargin: 5
        resizing: false


        // ###########################  IO PARAMETER FORM

        Rectangle {
            id : applicationForm
            x : parent.x + 5
            anchors.top : parent.top;
            width : parent.width
            height : 0

            Workbench.ApplicationForm {
                id : appFrame
                width : parent.width
                height : parent.height - 30 < 0 ?  0 : parent.height - 30
                opacity : 0
                z:1
            }
            MouseArea {
                id: appFrameMouseArea
                hoverEnabled: true
                anchors.fill: appFrame
                cursorShape: Qt.ArrowCursor
                property variant operationNameDrag
                drag.target: operationNameDrag

                onPressed: {
                    operationNameDrag = Qt.createQmlObject('import QtQuick 2.0; Text {
                        id : operationNameDrag
                        text : appFrame.formTitle
                        width : appFrame.width
                        height : appFrame.height
                        x : appFrame.x
                        y : appFrame.y
                        font.pointSize: 12
                        property string name :  appFrame.formTitle !== null ? appFrame.formTitle : ""
                        property string ilwisobjectid : appFrame.operationId !== null ? appFrame.operationId : ""
                        property string url : appFrame.operationUrl !== null ? appFrame.operationUrl : ""
                        property var frame : appFrame
                        property string type : "operation"

                        Drag.keys: [ appFrame.operationId ]
                        Drag.active: appFrameMouseArea.drag.active
                        Drag.hotSpot.x: x + width/2
                        Drag.hotSpot.y: y + height/2
                        opacity : Drag.active / 2

                        states: State {
                            when: appFrameMouseArea.drag.active
                            ParentChange { target: operationNameDrag; parent: root }
                            AnchorChanges { target: operationNameDrag; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
                        }
                    }', appFrameMouseArea, "dynamicOperationName");

                }

                onReleased: {
                    if (operationNameDrag !== null) {
                        operationNameDrag.Drag.drop()
                        operationNameDrag.parent = appFrameMouseArea
                        operationNameDrag.anchors.fill = appFrameMouseArea
                        operationNameDrag.destroy();
                    }
                }
                z:0
            }

            states: [
                State { name: "maximized"

                    PropertyChanges {
                        target: applicationForm
                        height : 300
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


        // ###########################  OPERATIONS

        TabView {
            id : operationTabs
            Tab {
                title : qsTr("Operation List")
                Workbench.OperationList {
                    id : operationList
                    Connections {
                        target : operationList
                        onMakeForm : {
                            newForm(objectid, name, url)
                        }
                    }
                }


            }
            Tab {
                title : qsTr("Operation Categories")
                Workbench.OperationCatagoriesList {
                    id : operationCatagories
                    Connections {
                        target : operationCatagories
                        onMakeForm : {
                            newForm(objectid, name, url)
                        }
                    }
                }

            }
            style: Base.TabStyle1{}
        }
    }


}
