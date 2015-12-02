import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import ".." as DataPanel

Rectangle {
    id : modelmanager
    width: parent.width
    height: 62
    Component {
        id : modelLayerOptions
        LayerManagement{}
    }

    Component {
        id : operationForms
        OperationForms{}
    }

    Component {
        id : runForms
        OperationForms{}
    }

    Component{
        id : metadataComp
        MetaData{
        }
    }

    /**
      Calls the newForm method and passes through the fields that should be hidden
      */
    function showOperationFormWithHiddenFields(operationid, itemId, hiddenFields){
        if ( operationid){
            datapane.state = "smaller"
            var tab= modellerProperties.getTab(1)
            tab.active=true
            tab.item.newOperationFormWithHiddenFields(operationid, qsTr("Set default values"), itemId,hiddenFields)
        }
    }

    function showOperationForm(operationid, itemId){
        if ( operationid){
            datapane.state = "smaller"
            var tab= modellerProperties.getTab(1)
            tab.active=true
            tab.item.newForm(operationid, qsTr("Set default values"), itemId)
        }
    }

    function showMetaData(item){
        var tab = modellerProperties.getTab(2)
        tab.item.setDesc(item.description)
        tab.item.setName(item.syntax)
        tab.item.setKeywords(item.keywords)
    }
    function resetMetaData(workflow){
        var tab = modellerProperties.getTab(2)
        tab.item.setDesc(workflow.description)
        tab.item.setName(workflow.name)
        tab.item.setKeywords(workflow.keywords)
    }

    /**
      * Shows the run form for the whole workflow
      */
    function showRunForm(workflowid){
        if ( workflowid){
            datapane.state = "smaller"
            var tab= modellerProperties.getTab(3)
            tab.active=true
            tab.item.newFormWithOutput(workflowid, qsTr("Set run values"))
        }
    }

    /**
      Calls the execute form methods of the OperationForms class, which returns the data the user put into the form.
      */
    function retrieveRunFormValues(){
        var tab= modellerProperties.getTab(3)
        return tab.item.executeForm()
    }

    TabView{
        id : modellerProperties
        anchors.fill: parent
        tabPosition: Qt.BottomEdge

        function tabClicked(index){
            if ( currentIndex === index){
                if ( modelmanager.height <= 60){
                    datapane.state = "smaller"
                }
                else{
                    datapane.state = ""
                    datapane.state = "bigger"
                }
            }

            currentIndex = index
        }


        Component.onCompleted: {
            var tab =addTab(qsTr("Model layers"), modelLayerOptions)
            tab.active = true

            addTab(qsTr("Operation Form"), operationForms)
            tab = addTab(qsTr("Metadata"), metadataComp)
            addTab(qsTr("Workflow Form"), runForms)
            tab.active = true // we need to be active as layers maybe added to it
        }

        style: DataPanel.ButtonBarTabViewStyle{}
    }



}

