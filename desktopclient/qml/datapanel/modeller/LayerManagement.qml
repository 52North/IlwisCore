import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Rectangle {
    id : modellayers
    width : parent.width
    height : parent.height

    function setLayerIndex(index){
        layerColumn.currentIndex = index
    }

    function getPropertyIndex() {
        return properties.currentIndex
    }

    function setEditor(editUrl){
        editor.source = editUrl
    }

    SplitView {
        id : layerprops
        property var layernames : ["Definition view","Operational view","Template view", "Workflow view"]
        width : parent.width - 5
        height : parent.height
        y : 2

        function getItems(){
            if ( layerColumn.currentIndex == 3){
                if ( model){
                    var wf = model.workflow("first_workflow_model") // atm we assume one workflow per scenario
                    if ( wf !== null)
                        return wf.propertyList()
                }
            }
            return null
        }
        handleDelegate: Controls.SplitHandle{
            imageHeight: 15
        }
        ModelLayers{
            id : layerColumn
            height : parent.height
            width : 130
            anchors.left: parent.left
            onCurrentIndexChanged: {
                datapaneChanged(currentIndex)
                if (!editor.item){
                    if ( currentIndex == 3){
                        editor.source = "workflow/WorkFlowForms.qml"
                    }
                }
            }
        }

        ModelLayerProperties{
            id : properties
            width : 160
            height : parent.height

            onCurrentIndexChanged: {
                if ( editor && editor.item){
                    editor.item.enable(properties.currentIndex, null)
                }
            }
        }

        ModelPropertyEditor{
            id : editor
            width : 400
            height : parent.height

            onStatusChanged: {
                if (editor.status === Loader.Ready) {
                    item.height = height
                    activeEditor = item
                    console.debug("done")
                }
            }
        }
    }
}

