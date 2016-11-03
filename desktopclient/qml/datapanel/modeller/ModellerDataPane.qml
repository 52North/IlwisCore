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
    property string panelType : "workflow"
    property ModelDesigner model
    property int ontTopZValue: 1000000
    property var createParameters : []
    property double factor : 1.1

    function addDataSource(filter, sourceName, sourceType){
        if ( filter !== "" ){
            if (sourceType === "workflow" ) {
                modellerDataPane.model = modelbuilder.createModel(modellerDataPane)
                var resource = mastercatalog.id2Resource(filter.split('=')[1],modellerDataPane);
                workflowView.workflow = model.addWorkflow(filter);
                model.currentWorkflow = workflowView.workflow
                //manager.setLayerIndex(3)
                if (resource) {
                   workflowView.drawFromWorkflow()
                }
                createParameters = [filter, sourceName, sourceType]


                manager.updateLists()
            }
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

        }else if ( index == 1){
            conceptualView.state = "invisible"
            workflowView.state = "invisible"
            analysisView.state = "visible"
            applicationView.state = "invisible"
        } else if ( index == 2){
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
    Sets the zoom percentage based on the tform's xScale
    */
    function setPercentage(){
        var scl = workflowView.getScale();
        modellertools.zoomLevel.text = Math.round((scl * 100)) + "%"
    }

    /**
      Calls the WorkflowCanvas's run method
      */
    function run() {
        workflowView.run()
    }

    function addError(id, error) {
        errorview.addError(id, error)
    }

    function toggleStepMode(){
        workflowView.workflow.toggleStepMode();
    }

    function workflowModel() {
        return workflowView.workflow
    }

    function nextStep() {
        workflowView.showLastSteppedItem()
    }

    signal exit;

    property bool canSeparate : true

    ModellerTools{
        id : modellertools
    }

    function setSelectedOperationId(metaid){
        datapane.setSelectedOperationId(metaid)
    }

     ModellerErrorView {
        height: 0
        id : errorview
        width : parent.width
        y: modellertools.height
        z: ontTopZValue
        color: Global.alternatecolor4
        border.width: 1
        border.color: Global.alternatecolor1

        states: [
            State {
                name : "bigger"
                PropertyChanges {
                    target: errorview
                    height : 80
                }
            },
            State {
                name: "smaller"
                PropertyChanges {
                    target: errorview
                    height : 0
                }
            }
        ]
        transitions: [
            Transition {
                NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
            }
        ]
    }

    SplitView {
        anchors.top : modellertools.bottom
        width : parent.width
        orientation: Qt.Vertical
        height : parent.height - modellertools.height
        id : modellersplit

        Item {
            id : datapane
            width : parent.width
            height : parent.height - 300

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

            WorkFlow.ModellerWorkflowView { id: workflowView }
            Analysis.ModellerAnalysisView{ id : analysisView}
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

        ModelManager{
            id : manager
            height : 340
            anchors.left: parent.left
            anchors.right: parent.right

        }
    }
    Component.onCompleted: {
        manager.workflowView= workflowView
        manager.analysView = analysisView
        manager.applicationView= applicationView
        manager.conceptView= conceptualView
    }
}
