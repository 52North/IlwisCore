import QtQuick 2.0
import QtQuick.Controls 1.0

import WorkflowCatalogModel 1.0

import ".." as Workbench

Rectangle {
    id: workflowcontainer

    signal unloadcontent(string content)

    Workbench.FunctionBarHeader {
        id : functionBarHeader
        width: parent.width
        headerImage: "../images/modellerCS1.png"
        headerText:qsTr("Workflows")
    }


    // ###########################  CONTENT LOADER

    Loader {
        id : workflowbenchContentLoader

        width : parent.width
        anchors.bottom: parent.bottom
        anchors.top : functionBarHeader.bottom
        Component.onCompleted : {
            if (workflows.hasActiveEditSession) {
                console.log("existing edit session: " + workflows.currentWorkflow)
                workflowbenchContentLoader.setSource("WorkflowEdit.qml")
            } else {
                console.log("new workflow bench")
                workflowbenchContentLoader.setSource("WorkflowBench.qml")
            }
        }

    }

}
