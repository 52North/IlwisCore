import QtQuick 2.0
import QtQuick.Controls 1.0

Rectangle {


    // ###########################  TITLE

    WorkflowBenchTitle {
        titleText: qsTr("Edit Workflow")
    }

    // ###########################  CONTROL BUTTONS

    Rectangle {
        id: workflowEditButtons
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
            onClicked : console.log("TODO cancel and go back")
            enabled: true
        }

        WorkflowSeparator { }
    }

    // ###########################  WORKFLOW'S IO SECTION

    // To the framwork a workflow shall appear just as an operation;
    // An operation provides OperationMetadata from where dynamic
    // IO form is being created. Here the same form can be used, but
    // would have to be updated, e.g. on canvas selection signals ...

}
