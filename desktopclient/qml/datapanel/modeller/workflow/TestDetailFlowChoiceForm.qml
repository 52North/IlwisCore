import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import Qt.labs.folderlistmodel 2.1
import "../../../Global.js" as Global
import "../../../controls" as Controls
import OperationModel 1.0

WorkflowChoiceForm {
    id : flowParameterForm
    property var conditionTo
    property var nodeFrom
    property var attachRect
    property int toIndex
    property var toType
    property int testIndex

    onNodeFromChanged: {
        fillInputModel()
    }

    function refresh() {
        wfCanvas.stopWorkingLine()
        flowParameterForm.state = "invisible"
        wfCanvas.canvasValid = false
        canvasActive = true
        fromModel.clear()
    }

    function fillInputModel() {
        fromModel.clear()
        var parameterIndexes = nodeFrom.operation.parameterIndexes(toType, false)

        for (var i in parameterIndexes) {
            var name = parameterIndexes[i]

            fromModel.append({'text': name})
        }
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
            text : qsTr("Connect parameters Indexes")
            font.bold: true
        }
        radius : 3
    }

    Item {
        id : outParams
        anchors.top : header.bottom
        anchors.topMargin: 4
        x : 5
        width : parent.width
        height : 20
        Text {
            id : outLabel
            height : 20
            width : 150
            text : qsTr("Outgoing parameters index")
            font.pointSize: 8
            y :3
        }

        ComboBox{
            id : fromComboBox
            anchors.left : outLabel.right
            height : 20
            width : parent.width - outLabel.width -10
            model : ListModel {
                id: fromModel
            }
        }
    }
    Button{
        text : qsTr("Cancel")
        height : 20
        width : 60
        anchors.right: applyButton.left
        anchors.rightMargin: 5
        anchors.top : applyButton.top
        onClicked: {
            refresh()
        }
    }
    Button{
        id : applyButton
        text : qsTr("Apply")
        height : 20
        width : 60
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.top : outParams.bottom
        anchors.topMargin: 5
        onClicked: {
            function getIndex(txt){
                var parts = txt.split(":")
                return parts[0]
            }

            var fromIndex = getIndex(fromComboBox.currentText)
            var flowPoints = { "fromParameterIndex" : fromIndex, "toParameterIndex" : toIndex};
            nodeFrom.setFlow(conditionTo, conditionTo.att1, flowPoints, testIndex)
            conditionTo.setValue(nodeFrom.itemid, fromIndex, toIndex)
            refresh()
        }
    }


    states: [
        State { name: "visible"

            PropertyChanges {
                target: flowParameterForm
                enabled : true
                height : 120
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: flowParameterForm
                enabled : false
                height : 0
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

