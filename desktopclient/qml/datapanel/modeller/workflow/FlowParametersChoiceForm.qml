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
    property var nodeFrom
    property var nodeTo
    property var attachRectTo
    property var target
    property int fromParameterIndex : 0
    property int toParameterIndex : 0
    property string typeFilter : ""

    onNodeToChanged: {
       
        if ( nodeFrom && nodeTo){
            var outParameterNames = [], count=0
            if ( nodeTo.type === "junctionitem"){
                outParameterNames = nodeFrom.parameterNames(typeFilter,true)

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
            if (nodeFrom.type === "operationitem"){
                for (var i in inParameterNames) {
                    type = nodeTo.parameterType(i,false)
                    if (validTypes.indexOf(type) === -1) {
                        validTypes.push(type)
                        outParameterNames = nodeFrom.parameterNames(type,true)
                        count += outParameterNames.length

                        for (var j in outParameterNames) {
                            outModel.append({'text': outParameterNames[j]})
                        }
                    }
                }
                if (count === 0) {
                    modellerDataPane.addError(1, 'There were no possible matches found between ' + nodeFrom.itemid + ' and ' + nodeTo.itemid)
                    refresh()
                } else {
                    fillInputModel(nodeFrom.parameterType(fromParameterIndex,true))
                }
            }else{
                outModel.clear()
                fillInputModel(nodeFrom.dataType)
            }
        }
    }

    function refresh() {
        wfCanvas.stopWorkingLine()
        flowParameterForm.state = "invisible"
        wfCanvas.canvasValid = false
        canvasActive = true
        if ( nodeFrom)
            nodeFrom.resetColors()
        if ( nodeTo)
            nodeTo.resetColors();
        nodeFrom = null
        nodeTo = null
        attachRectTo = 0
        inModel.clear()
        outModel.clear()
    }

    function fillInputModel(type) {
        if (nodeTo) {
            inModel.clear()
            var parameterName = nodeTo.parameterNames(type, false)

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
            opacity : outModel.count == 0 ? 0 : 1
            enabled : opacity
            model : ListModel {
                id: outModel
            }
            onCurrentIndexChanged: {
                if ( nodeFrom){
                    fillInputModel(nodeFrom.parameterType(currentIndex))
                }
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
                return Number(parts[0])
            }
            fromParameterIndex = getIndex(outComboBox.currentText)
            if ( !nodeTo)
                return
            if ( nodeTo.type === "operationitem")
                toParameterIndex = getIndex(inComboBox.currentText)
            else{
                toParameterIndex = nodeTo.containedInLinkedCondition(nodeFrom) ? 1 : 2 // 0=condition, 1=truecase, 2=falsecase
                nodeTo.dataType = nodeFrom.parameterType(fromParameterIndex, true)
            }
            var flowPoints = { "fromParameterIndex" : fromParameterIndex, "toParameterIndex" : toParameterIndex};

            if (!nodeFrom) {
                if ( workarea.currentItem.type === "operationitem")
                    nodeFrom = workarea.currentItem
            }
            nodeFrom.setFlow(nodeTo, attachRectTo, flowPoints,nodeTo.type === "operationitem" ? -1 : -2)
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

