import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global

Rectangle {
    id : modellayers
    width : parent.width
    height : parent.height
    property string iconName : "../images/layers_s"

    SplitView {
        id : layerprops
        width : parent.width - 5
        height : parent.height
        y : 2

        Workflows{
            id : workflows
            height : parent.height
            width : 150
            anchors.left: parent.left
            onCurrentIndexChanged: {
                if ( modellerDataPane.model){
                    var wf = modellerDataPane.model.workflow(currentIndex)
                    modellerDataPane.setCurrentWorkflow(wf,currentIndex);
                }
            }
        }

        WorkflowDataForms {
            id : dataForm
            width:100
            Layout.fillWidth: true
            height : parent.height
        }

        WorkflowCreateForm {
            id : createForm
            state : "invisible"
            height : parent.height
        }
    }

    Component.onCompleted: {
        updateLists(0)
    }

    function updateLists(currentIndex) {
        if ( modellerDataPane.model){
            if ( currentIndex != workflows.currentIndex || workflows.workflowNamesModel == null){
                workflows.workflowNamesModel = null
                workflows.workflowNamesModel = modellerDataPane.model.workflowNames
            }
        }
    }

    function executeRunForm(runparms){
        return dataForm.executeRunForm(runparms)
    }

    function showSelectedOperation(item) {
        dataForm.showSelectedOperation(item)
    }

    function updateRunForm() {
        dataForm.updateRunForm()
    }

    function updateMetadata() {
        dataForm.updateMetadata()
    }

    function stepMode() {
//            if ( model){
//                var wf = model.workflow(workflows.currentIndex) // atm we assume one workflow per scenario
//                if ( wf)
//                    return wf.gotoStepMode()
//            }
    }
}

