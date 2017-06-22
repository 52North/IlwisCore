import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import ".." as DataPanel
import "./analysisview" as Analysis
import "./conceptualview" as Concepts
import "./applicationview" as Apps
import "./workflow" as WorkFlow

Rectangle {
    id : modelmanager
    width: parent.width
    property var activeEditor
    property var workflowView
    property var analisysView
    property var applicationView
    property var conceptView
    property alias analysisManager : analysisManager
    property alias workflowManager : wfManager.item

    TabView{
        id : modellerViews
        anchors.fill: parent
        tabPosition: Qt.BottomEdge

        function tabClicked(index){
            if ( workflowOnly){
                if ( index !== 3)
                    return
            }
            var newState = datapane.state
            if ( currentIndex === index){
                if ( modelmanager.height <= 60){
                    newState = "smaller"
                }
                else{
                    newState = "bigger"
                }
            }
            currentIndex = index
            datapane.changeDataPane(index, newState)
        }

        Tab {
            id : conceptManager
            active: true
            title: qsTr("Conceptual View")
            Concepts.ManagerConceptualView{}
        }

        Tab{
            id : applicationManager
            active: true
            title: qsTr("App Wizard")
            Apps.ManagerApplicationView{}
        }

        Tab{
            id : analysisManager
            active: true
            title: qsTr("Specialist editor")
            Analysis.ManagerAnalysisView{}
        }

        Tab {
            id : wfManager
            active: true
            title: qsTr("Workflow Builder")
            WorkFlow.ManagerWorkflowView{}
        }

        style: DataPanel.ButtonBarTabViewStyle{}
    }

    function setLayerIndex(index){
        workflowManager.item.setLayerIndex(index)
    }

    /**
      Calls the newForm method and passes through the fields that should be hidden
      */
    function showForm(parms){
        if ( activeEditor){
            activeEditor.enable(workflowManager.item.getPropertyIndex(), parms)
        }
    }

    function showConditionForm(operationId, hiddenFields, constantValues, conditionIds){
        if ( activeEditor)
            activeEditor.showConditionForm(operationId, hiddenFields, constantValues, conditionIds)
    }


    function retrieveRunFormValues(){
        return forms.item.retrieveRunFormValues()
    }

    function clearOperationForm() {
        if ( activeEditor)
            activeEditor.clearOperationForm()
    }

    function selectedWorkflowItem(itemid){
        //workflowManager.item.selectedWorkflowItem(itemid)
    }

    function updateLists(subcurrentIndex){
        if ( modellerDataPane.model ){
            if ( modellerDataPane.model.conceptCount > 0)
                modellerViews.currentIndex = 0
            else if ( modellerDataPane.model.applicationCount > 0){
                modellerViews.currentIndex = 1
                applicationView.currentApplication = modellerDataPane.model.application(currentIndex)
                analysisView.state = workflowView.state = "invisible"
            }else if ( modellerDataPane.model.analysisCount > 0){
                modellerViews.currentIndex = 2
                var analysis = modellerDataPane.model.analysisPattern(subcurrentIndex)
                analisysView.currentAnalysis = analysis
                analysisManager.item.refreshFormArea(analysis.panel("form"))
                applicationView.state = workflowView.state = "invisible"
            } else if ( modellerDataPane.model.workflowCount > 0){
                modellerViews.currentIndex = 3
                workflowView.workflow = modellerDataPane.model.workflow(subcurrentIndex)
                applicationView.state = analisysView.state = "invisible"
                workflowManager.updateLists(subcurrentIndex)
            }
            else
                modellerViews.currentIndex = 0
        }
    }
}

