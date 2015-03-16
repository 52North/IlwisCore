import QtQuick 2.0
import WorkflowModel 1.0

Rectangle {

    property var workflowModel : workflows.workflows
    property bool byKey : false

    width : parent.width
    color : "white"
    clip : true
    ListView {
        id: workflowlist
        model : workflowModel
    }
}
