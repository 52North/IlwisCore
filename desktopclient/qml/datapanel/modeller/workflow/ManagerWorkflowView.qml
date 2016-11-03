import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global
import "./forms" as Forms

Rectangle {
    id : modellayers
    width : parent.width
    height : parent.height
    property string iconName : "../images/layers_s"

    function setLayerIndex(index){
        workflows.currentIndex = index
    }

    function getPropertyIndex() {
        return properties.currentIndex
    }

    function setEditor(editUrl){
        editor.source = editUrl
    }

    function selectedWorkflowItem(itemid){
        if ( itemid >= 0)
            properties.currentIndex = 1
        else
            properties.currentIndex = 0
    }

    function updateLists() {
        workflows.workflowNamesModel = modellerDataPane.model.workflowNames
        properties.propertyItems = layerprops.getItems();
    }

    SplitView {
        id : layerprops
        width : parent.width - 5
        height : parent.height
        y : 2

        function getItems(){
            if ( modellerViews.currentIndex == 3){
                if ( model){
                    var wf = model.workflow(workflows.currentIndex) // atm we assume one workflow per scenario
                    if ( wf !== null)
                        return wf.propertyList()
                }
            }
            return null
        }

        function stepMode() {
            if ( model){
                var wf = model.workflow(workflows.currentIndex) // atm we assume one workflow per scenario
                if ( wf)
                    return wf.gotoStepMode()
            }
        }

        handleDelegate: Controls.SplitHandle{
            imageHeight: 15
        }
        Workflows{
            id : workflows
            height : parent.height
            width : 130
            anchors.left: parent.left
            onCurrentIndexChanged: {
                if ( modellerDataPane.model){
                    var wf = modellerDataPane.model.workflow(currentIndex)
                    console.debug("a", currentIndex, wf, workflowView)
                    if ( wf && workflowView){
                        workflowView.workflow = wf
                    }
                }
            }
        }

        WorkflowProperties{
            id : properties
            width : 160
            height : parent.height

            onCurrentIndexChanged: {
                if ( editor && editor.item ){
                    editor.item.enable(properties.currentIndex, null)
                }
            }
        }

        WorkflowPropertyEditor{
            id : editor
            width : 400
            height : parent.height

            onStatusChanged: {
                if (editor.status === Loader.Ready) {
                    item.height = height
                    activeEditor = item
                }
            }
        }
    }
    Component.onCompleted: {
        editor.source = "WorkFlowForms.qml"
    }
}

