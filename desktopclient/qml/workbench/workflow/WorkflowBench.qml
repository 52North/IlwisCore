import QtQuick 2.0
import WorkflowModel 1.0

import "../../workbench" as Workbench

Rectangle {
    id: workflowbench

    signal unloadcontent(string content)


    Workbench.FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/modellerCS1.png"
        headerText:qsTr("Workflows")
    }

    Connections {
         target: workflowlist
         onWorkflowsChanged: { refreshWorkflowList()}
     }

    function refreshWorkflowList() {
        workflowlist.workflowmodel = workflow.bookmarked
    }

    WorkflowList {
        id: workflowlist

    }


}
