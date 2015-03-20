import QtQuick 2.0
import QtQuick.Controls 1.0

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

        property bool activeEditSession : false
        property string editSession

        width : parent.width
        anchors.top : functionBarHeader.bottom
        anchors.bottom: parent.bottom
        Component.onCompleted : {
            if ( !activeEditSession) {
                workflowbenchContentLoader.setSource("WorkflowBench.qml")
            } else {
                workflowbenchContentLoader.setSource("WorkflowEdit.qml")
            }
        }

    }

}
