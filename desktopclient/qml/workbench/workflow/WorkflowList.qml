import QtQuick 2.0

import OperationModel 1.0
import WorkflowCatalogModel 1.0
import WorkflowModel 1.0

import "../../Global.js" as Global

Rectangle {

    property var workflowModel : workflows.workflows
    property var operationModel : operations.operations

    width : parent.width
    color : "white"
    clip : true

    ListView {
        id: listview
        anchors.fill: parent
        model : workflowModel

        delegate : Item {
            id : currentWorkflow
            height : 35;
            width : parent.width;
            Rectangle {
                anchors.fill: parent
                color: listview.currentIndex === index ? Global.selectedColor : index  % 2 == 0 ? "#F7F9FC" : "#DCDCDC"
                Text {
                    id : workflowName
                    text : displayName
                    anchors.top : parent.top
                    width : parent.width
                    height : 17
                    x : 5
                    font.pointSize: 12
                }

                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    cursorShape: Qt.ArrowCursor
                    onClicked: {
                        listview.currentIndex = index;
                        createWorkflowMetadataForm(id, displayName);
                    }
                }

            }

        }
    }
}
