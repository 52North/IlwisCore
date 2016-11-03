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

    function setLayerIndex(index){
        workflowView.item.setLayerIndex(index)
    }

    /**
      Calls the newForm method and passes through the fields that should be hidden
      */
    function showForm(parms){
        if ( activeEditor){
            activeEditor.enable(workflowView.item.getPropertyIndex(), parms)
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
        workflowView.item.selectedWorkflowItem(itemid)
    }

    function updateLists(){
        if ( modellerDataPane.model ){
            if ( modellerDataPane.model.conceptCount > 0)
                modellerViews.currentIndex = 0
            else if ( modellerDataPane.model.applicationCount > 0)
                modellerViews.currentIndex = 1
            else if ( modellerDataPane.model.analysisCount > 0)
                modellerViews.currentIndex = 2
            else if ( modellerDataPane.model.workflowCount > 0)
                modellerViews.currentIndex = 3
            else
                modellerViews.currentIndex = 0
        }
        workflowView.item.updateLists()
    }

    TabView{
        id : modellerViews
        anchors.fill: parent
        tabPosition: Qt.BottomEdge

        function tabClicked(index){
            if ( currentIndex === index){
                if ( modelmanager.height <= 60){
                    datapane.state = "smaller"
                }
                else{
                    datapane.state = "bigger"
                }
            }
            currentIndex = index
        }

        Tab {
            id : conceptView
            title: qsTr("Conceptual View")
            active: true
            Concepts.ConceptualView{}

        }

        Tab{
            id : applicationView
            active: true
            title: qsTr("Application View")
            Apps.ApplicationView{}
        }

        Tab{
            id : analysisView
            active: true
            title: qsTr("Analysis View")
            Analysis.AnalysisView{}
        }
        Tab{
            id : workflowView
            active: true
            title: qsTr("Workflow View")
            WorkFlow.WorkflowView{}
        }


        style: DataPanel.ButtonBarTabViewStyle{}

    }



}

