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
    property var operationFrom
    property var nodeTo
    property var attachRectTo
    property var target
    property int fromParameterIndex : -1
    property int toParameterIndex : -1
    property string typeFilter : ""

    onNodeToChanged: {
        if ( operationFrom && nodeTo){
            var outParameterNames = [], count=0
            if ( nodeTo.type === "junctionitem"){
                outParameterNames = operationFrom.parameterNames(type,true)

                count = outParameterNames.length

                for (var j in outParameterNames) {
                    outModel.append({'text': outParameterNames[j]})
                }
                return
            }
            var inParameterNames = nodeTo.parameterNames(typeFilter,false),
                validTypes = [],
                type

            // Get valid types
            for (var i in inParameterNames) {
                type = nodeTo.parameterType(i,false)
                if (validTypes.indexOf(type) === -1) {
                    validTypes.push(type)
                    outParameterNames = operationFrom.parameterNames(type,true)

                    count += outParameterNames.length

                    for (var j in outParameterNames) {
                        outModel.append({'text': outParameterNames[j]})
                    }
                }
            }

            if (count === 0) {
                modellerDataPane.addError(1, 'There were no possible matches found between ' + operationFrom.itemid + ' and ' + nodeTo.itemid)
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
        operationFrom = null
        nodeTo = null
        attachRectTo = 0
        inModel.clear()
        outModel.clear()
    }

    function fillInputModel(index) {
        if (nodeTo) {
            inModel.clear()
            var parameterName = nodeTo.parameterNames(operationFrom.parameterType(index), false)

            for (var i in parameterName) {
                var name = parameterName[i]

                if (!workarea.workflow.hasValueDefined(nodeTo.itemid, name.split(':')[0])) {
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
        opacity : (nodeTo && nodeTo.type === "operationitem") ? 1 : 0
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
        anchors.top : inParams.bottom
        anchors.topMargin: 5
        onClicked: {
            function getIndex(txt){
                var parts = txt.split(":")
                return parts[0]
            }
            fromParameterIndex = getIndex(outComboBox.currentText)
            if ( nodeTo.type === "operationitem")
                toParameterIndex = getIndex(inComboBox.currentText)
            else{
                toParameterIndex = nodeTo.containedInLinkedCondition(operationFrom) ? 1 : 2 // 0=condition, 1=truecase, 2=falsecase
                nodeTo.dataType = operationFrom.parameterType(fromParameterIndex, true)
            }
            var flowPoints = { "fromParameterIndex" : fromParameterIndex, "toParameterIndex" : toParameterIndex};

            if (!operationFrom) {
                if ( workarea.currentItem.type === "operationitem")
                    operationFrom = workarea.currentItem
            }
            operationFrom.setFlow(nodeTo, attachRectTo, flowPoints,nodeTo.type === "operationitem" ? -1 : -2)
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

