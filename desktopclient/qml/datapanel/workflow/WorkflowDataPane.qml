import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.1

// Window does not work!
//Window {
Item {
    id: workflowDataPane
    width : parent.width
    height : parent.height

    signal exit;

    property bool canSeparate : true

    property string workflow;

    WorkflowCanvas {
        id: canvas
        anchors.fill: workflowDataPane
        workflow:  workflowDataPane.workflow;
    }

    Component.onDestruction: {
        exit();
    }
}
