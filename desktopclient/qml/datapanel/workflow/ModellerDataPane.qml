import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import TabModel 1.0

// Window does not work!
//Window {
Item {
    id: workflowDataPane
    width : parent.width
    height : parent.height
    property TabModel tabmodel

    function addDataSource(sourceUrl, sourceName, sourceType){
        if ( sourceUrl !== ""){
            //TODO line the workflow to the workflow model
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

    function datapaneChanged(index){
        if ( index == 0) {
            defview.state = "visible"
            canvas.state = "invisible"
            operview.state = "invisible"
            templateBuilder.state = "invisible"

        }else if ( index == 1){
            defview.state = "invisible"
            canvas.state = "invisible"
            operview.state = "visible"
            templateBuilder.state = "invisible"
        } else if ( index == 2){
            defview.state = "invisible"
            canvas.state = "invisible"
            operview.state = "invisible"
            templateBuilder.state = "visible"
        }else{
            defview.state = "invisible"
            canvas.state = "visible"
            operview.state = "invisible"
            templateBuilder.state = "invisible"
        }
    }

    signal exit;

    property bool canSeparate : true
    property string panelType : "workflow"

    property string workflow;



    ModellerTools{
        id : modellertools
    }

    SplitView {
        anchors.top : modellertools.bottom
        width : parent.width
        orientation: Qt.Vertical
        height : parent.height - modellertools.height

        Item {
            width : parent.width
            height : parent.height - modellertools.height - 170
            WorkflowCanvas {
                id: canvas
                state : "visible"
            }
            ModellerDefinitionView{ id : defview}
            ModellerTemplateBuilder{ id : templateBuilder}
            ModellerOperationalView{ id : operview}
        }
        ModelManager{
            height : 170
        }


    }

}
