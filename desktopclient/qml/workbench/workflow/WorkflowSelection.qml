import QtQuick 2.0
import QtQuick.Controls 1.0

import ".." as Workbench

Rectangle {
    id: workflowcontainer

    property bool activeEditSession : false

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

        property string editSession

        width : parent.width
        anchors.bottom: parent.bottom
        anchors.top : functionBarHeader.bottom
        Component.onCompleted : {
            if ( !activeEditSession) {
                console.log("new workflow bench")
                workflowbenchContentLoader.setSource("WorkflowBench.qml")
            } else {
                console.log("existing edit session: " + editSession)
                workflowbenchContentLoader.setSource("WorkflowEdit.qml")
            }
        }

    }

}
