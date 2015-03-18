import QtQuick 2.0
import QtQuick.Controls 1.0

import ".." as Workbench

Rectangle {
    id: workflowcontainer

    signal unloadcontent(string content)

    Workbench.FunctionBarHeader{
        id : functionBarHeader
        width: workflowcontainer.width
        headerImage: "../images/modellerCS1.png"
        headerText:qsTr("Workflows")
    }

    WorkflowBench {
        x: parent.x
        width : functionBarHeader.width
        anchors.top : functionBarHeader.bottom

    }


}
