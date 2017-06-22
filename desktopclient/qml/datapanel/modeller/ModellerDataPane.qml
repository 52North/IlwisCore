import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import TabModel 1.0
import WorkflowModel 1.0
import ModelBuilder 1.0
import ModelDesigner 1.0
import "./workflow" as WorkFlow
import "../../Global.js" as Global
import "./analysisview" as Analysis
import "./conceptualview" as Concepts
import "./applicationview" as Apps
import "./workflow" as WorkFlow

Item {
    id: modellerDataPane
    width : parent.width
    height : parent.height
    property TabModel tabmodel
    property string panelType : "model"
    property ModelDesigner model
    property int ontTopZValue: 1000000
    property var createParameters : []
    property double factor : 1.1
    property bool canSeparate : true
    property bool workflowOnly: false

    signal exit;

    SplitView {
        width : parent.width
        orientation: Qt.Vertical
        height : parent.height
        id : modellersplit

        Item {
            id : datapane
            width : parent.width
            height : parent.height - 340

            function changeDataPane(index, newState){
                datapane.state = newState
                modellerDataPane.datapaneChanged(index)
            }

            function asignConstantInputData(vertexIndex, parameterIndex, value){
                workflowView.asignConstantInputData(vertexIndex, parameterIndex, value)
            }
            function setSelectedOperationId(metaid){
                workflowView.workflow.setSelectedOperationId(metaid)
            }

            CopyAsBox {
                id : savestuff2
                y : 31
                state : "invisible"
                clip:true
                z : 1000
                ilwisobject : model
            }

            WorkFlow.ModellerWorkflowView { id: workflowView }
            Analysis.ModellerAnalysisView{ id : analysisView} // panel
            Apps.ModellerApplicationView{ id : applicationView}
            Concepts.ModellerConceptualView{ id : conceptualView}

            states: [
                State { name: "smaller"
                    PropertyChanges {
                        target: datapane
                        height : parent.height - 340
                    }
                },
                State {
                    name : "bigger"
                    PropertyChanges {
                        target: datapane
                        height : parent.height - 54
                    }
                }

            ]
            transitions: [
                Transition {
                    NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
                }
            ]
        }
        // form
        ModelManager{
            id : manager
            height : 340
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }

    Component.onCompleted: {
        manager.workflowView= workflowView
        manager.analisysView = analysisView
        manager.applicationView= applicationView
        manager.conceptView= conceptualView
        workflowView.workflowManager = manager.workflowManager
    }

    function addDataSource(filter, sourceName, sourceType){
        if ( filter !== "" ){

            if (sourceType === "model" ) {
                var resource = mastercatalog.id2Resource(filter.split('=')[1],modellerDataPane);
                if ( resource){
                    modellerDataPane.model = modelbuilder.createModel(resource, modellerDataPane)
                    if ( modellerDataPane.model){
                        if ( resource.typeName === "workflow"){
                            workflowView.workflow = model.addWorkflow(filter);
                            workflowOnly = true
                        }
                        model.currentWorkflow = workflowView.workflow
                        if (resource) {
                            workflowView.recreateWorkflow()
                        }
                        createParameters = [filter, sourceName, sourceType]
                        manager.updateLists(0)
                    }
                }
            }
        }
    }

    function setCurrentWorkflow(wf, currentIndex) {
         if ( wf && wf.name !== model.currentWorkflow.name){
             workflowView.clearCurrent()
             workflowView.workflow = wf
             model.currentWorkflow = workflowView.workflow
             if (model.currentWorkflow) {
                 workflowView.recreateWorkflow()
             }
             createParameters = ['itemid=' + wf.id, wf.name, 'workflow']
             manager.updateLists(currentIndex)
         }
     }

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../../images/" + name
        return iconP
    }

    function store() {
         workflowView.store()
     }

    function selectedWorkflowItem(itemid){
        manager.selectedWorkflowItem(itemid)
    }

    function datapaneChanged(index){
        if ( index == 0) {
            conceptualView.state = "visible"
            workflowView.state = "invisible"
            analysisView.state = "invisible"
            applicationView.state = "invisible"

        }else if ( index == 2){
            conceptualView.state = "invisible"
            workflowView.state = "invisible"
            analysisView.state = "visible"
            applicationView.state = "invisible"
        } else if ( index == 1){
            conceptualView.state = "invisible"
            workflowView.state = "invisible"
            analysisView.state = "invisible"
            applicationView.state = "visible"
        }else{
            conceptualView.state = "invisible"
            workflowView.state = "visible"
            analysisView.state = "invisible"
            applicationView.state = "invisible"
        }
    }

    function newCondition() {
        workflowView.newCondition()
    }

    function deleteSelectedOperation(){
        workflowView.deleteSelectedOperation()
    }

    function deleteSelectedEdge(){
        workflowView.deleteSelectedEdge()
    }

    function alterSelectedEdge(){
        workflowView.alterSelectedEdge()
    }

    function canvasZoom(clicks){
        workflowView.zoom(clicks)
    }

    /**
    Sets the canvas' zoom back to 100%
    */
    function defaultZoom(){
        workflowView.defaultZoom();
    }

    /**
      Calls the WorkflowCanvas's run method
      */
    function run() {
        workflowView.run()
    }

    function addError(id, error) {
        //errorview.addError(id, error)
    }

//    function toggleStepMode(){
//        workflowView.workflow.toggleStepMode();
//    }

    function workflowModel() {
        return workflowView.workflow
    }

    function setSelectedOperationId(metaid){
        datapane.setSelectedOperationId(metaid)
    }

}
