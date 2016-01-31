import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import Qt.labs.folderlistmodel 2.1
import "../../../../Global.js" as Global
import "../../../../controls" as Controls
import OperationModel 1.0

WorkflowChoiceForm {
    id : conditionTypeForm
    width: 200
    height: 80

    property var containerId

    function reset() {
        text.text = ''
        conditionTypeForm.state = "invisible"
        wfCanvas.canvasValid = false
        canvasActive = true
        mastercatalog.currentCatalog.filter("")
        filter.checked = false;
    }

    Rectangle {
        id : header
        width : parent.width
        height : 20

        color : Global.headerlight
        Text {
            x : 5
            y : 3
            width : 100
            height : 18
            text : qsTr("Choose condition type")
            font.bold: true
        }
        radius : 3
    }

    Item {
        id : conditionType
        anchors.top : header.bottom
        anchors.topMargin: 4
        x : 5
        width : parent.width
        height : 20
        Text {
            id : label
            height : 20
            width : 70
            text : qsTr("Operation:")
            font.pointSize: 8
            y :3
        }
        DropArea {
            id : operationDrop
            anchors.left : label.right
            height: 20
            width: parent.width - label.width - 30
            onDropped: {
                if ((drag.source.type === "singleoperation" || drag.source.type === "workflow")) {
                    var message = drag.source.message,
                        parts = message.split('/')

                    parts = parts[parts.length-1].split('=')

                    text.text = parts[0]
                    text.operationId = drag.source.ilwisobjectid
                }
            }
            TextField {
                id : text
                anchors.fill : parent
                property int operationId: -1
            }
        }
        Button{
            id: filter
            width: 20
            height: 20
            checkable : true
            checked : false
            anchors.left : operationDrop.right

            onClicked : {
                if (checked) {
                    mastercatalog.currentCatalog.filter("type=262144 and pout_1_type='16777216'")
                } else {
                    mastercatalog.currentCatalog.filter("")
                }
            }
            Image {
                anchors.centerIn: parent
                width: 14
                height: 14
                source: "../../../../images/operation20.png"
                fillMode: Image.PreserveAspectFit
            }
        }

    }
    Button{
        id : conditionCreateButton
        text : qsTr("Create")
        height : 20
        width : 60
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.top : conditionType.bottom
        anchors.topMargin: 5
        onClicked: {
            var info = canvas.workflow.addCondition(containerId, text.operationId)
            wfCanvas.conditionBoxList[containerId].addCondition(info.conditionId, info.name)
            reset()
        }
    }
    Button{
        text : qsTr("Cancel")
        height : 20
        width : 60
        anchors.right: conditionCreateButton.left
        anchors.rightMargin: 5
        anchors.top : conditionCreateButton.top
        onClicked: {
            reset()
        }
    }

    states: [
        State { name: "visible"

            PropertyChanges {
                target: conditionTypeForm
                enabled : true
                height : 80
            }
            PropertyChanges {
                target: wfCanvas.bigDropArea
                enabled : false
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: conditionTypeForm
                enabled : false
                height : 0
            }
            PropertyChanges {
                target: wfCanvas.bigDropArea
                enabled : true
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation {
                properties: "height"; duration : 200 ; easing.type: Easing.Linear
            }

        }
    ]
}

