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
    height: 300
    property var activeEditor

    function setLayerIndex(index){
        modelLayerOptions.item.setLayerIndex(index)
    }

    /**
      Calls the newForm method and passes through the fields that should be hidden
      */
    function showForm(parms){
        if ( activeEditor){
            activeEditor.enable(modelLayerOptions.item.getPropertyIndex(), parms)
        }
    }

    function showConditionForm(operationId, hiddenFields, constantValues, conditionIds){
        if ( activeEditor)
            activeEditor.showConditionForm(operationId, hiddenFields, constantValues, conditionIds)
    }


    function retrieveRunFormValues(){
        return forms.item.retrieveRunFormValues()
    }

    function showMetaData(item) {
        metadata.item.showMetaData(item)
    }

    function resetMetaData() {
        metadata.item.resetMetaData()
    }

    function showWorkflowMetadata(workflow) {
        metadata.item.showWorkflowMetaData(workflow)
    }

    function clearOperationForm() {
        if ( activeEditor)
            activeEditor.clearOperationForm()
    }

    function selectedWorkflowItem(itemid){
        modelLayerOptions.item.selectedWorkflowItem(itemid)
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
                    datapane.state = "bigger"
                }
            }

            currentIndex = index
        }


        Tab {
            id : modelLayerOptions
            title: qsTr("Model layers")
            active: true
            LayerManagement{
                id : layermanagement
            }
        }

        Tab{
            id : metadata
            active: true
            title: qsTr("Metadata")
            MetaDataTab{}
        }

        style: DataPanel.ButtonBarTabViewStyle{}
    }



}

