import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import Qt.labs.folderlistmodel 2.1
import "../../../../Global.js" as Global
import "../../../../controls" as Controls
import OperationModel 1.0

WorkflowChoiceForm {
    id : flowParameterForm
    property var operationOut
    property var operationIn
    property var attachRect
    property var target
    property var source

    onOperationInChanged: {
        if ( operationOut && operationIn){
            var inParameterIndexes = operationIn.operation.parameterIndexes("",false),
                outParameterIndexes = [],
                count = 0,
                validTypes = [],
                type

            // Get valid types
            for (var i in inParameterIndexes) {
                type = operationIn.operation.inputparameterType(i, false)
                if (validTypes.indexOf(type) === -1) {
                    validTypes.push(type)
                    outParameterIndexes = operationOut.operation.parameterIndexes(type,true)

                    count += outParameterIndexes.length

                    for (var j in outParameterIndexes) {
                        outModel.append({'text': outParameterIndexes[j]})
                    }
                }
            }

            if (count === 0) {
                modellerDataPane.addError(1, 'There were no possible matches found between ' + operationOut.name.text + ' and ' + operationIn.name.text)
                refresh()
            } else {
                fillInputModel(0)
            }
        }
    }

    function refresh() {
        wfCanvas.stopWorkingLine()
        flowParameterForm.state = "invisible"
        wfCanvas.canvasValid = false
        canvasActive = true
        source = null
        operationOut = null
        operationIn = null
        attachRect = 0
        target = null
        inModel.clear()
        outModel.clear()
    }

    function fillInputModel(index) {
        if (operationIn) {
            inModel.clear()
            var parameterIndexes = operationIn.operation.parameterIndexes(operationOut.operation.inputparameterType(index), false)

            for (var i in parameterIndexes) {
                var name = parameterIndexes[i]

                if (!canvas.workflow.hasValueDefined(operationIn.itemid, name.split(':')[0])) {
                    inModel.append({'text': name})
                }
            }
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
            id : outComboBox
            anchors.left : outLabel.right
            height : 20
            width : parent.width - outLabel.width -10
            model : ListModel {
                id: outModel
            }
            onCurrentIndexChanged: {
                fillInputModel(currentIndex)
            }
        }
    }
    Item {
        id : inParams
        anchors.top : outParams.bottom
        anchors.topMargin: 4
        x : 5
        width : parent.width
        height : 20
        Text {
            id : labelIn
            height : 20
            width : 150
            text : qsTr("Input parameters index")
            font.pointSize: 8
            y :3
        }

        ComboBox{
            id : inComboBox
            anchors.left : labelIn.right
            height : 20
            width : parent.width - labelIn.width -10
            model : ListModel {
                id: inModel
            }
        }
    }
    Button{
        text : qsTr("Apply")
        height : 20
        width : 60
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.top : inParams.bottom
        anchors.topMargin: 5
        onClicked: {
            function getIndex(txt){
                var parts = txt.split(":")
                return parts[0]
            }

            var fromIndex = getIndex(outComboBox.currentText)
            var toIndex = getIndex(inComboBox.currentText)
            var flowPoints = { "fromParameterIndex" : fromIndex, "toParameterIndex" : toIndex};

            if (!source) {
                source = wfCanvas.operationsList[wfCanvas.currentIndex]
            }
            source.setFlow(target, attachRect, flowPoints)
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

