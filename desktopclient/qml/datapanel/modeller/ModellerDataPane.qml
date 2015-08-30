import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import TabModel 1.0
import WorkflowModel 1.0
import ScenarioBuilderModel 1.0
import ScenarioDesignerModel 1.0
import "./workflow" as WorkFlow

Item {
    id: modellerDataPane
    width : parent.width
    height : parent.height
    property TabModel tabmodel
    property ScenarioDesignerModel scenario

    function addDataSource(filter, sourceName, sourceType){
        if ( filter !== "" ){
            if ( sourceType === "workflow")            {
                scenario = scenarios.create()
                var wf = scenario.addWorkflow(filter)
                canvas.workflow = wf;
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


    ModellerTools{
        id : modellertools
    }

    SplitView {
        anchors.top : modellertools.bottom
        width : parent.width
        orientation: Qt.Vertical
        height : parent.height - modellertools.height

        Item {
            id : datapane
            width : parent.width
            height : parent.height - modellertools.height - 170


            WorkFlow.WorkflowCanvas {
                id: canvas
                state : "visible"
            }
            ModellerDefinitionView{ id : defview}
            ModellerTemplateBuilder{ id : templateBuilder}
            ModellerOperationalView{ id : operview}

            states: [
                State { name: "smaller"

                    PropertyChanges {
                        target: datapane
                        height : parent.height - modellertools.height - 170
                    }
                },
                State {
                    name : "bigger"
                    PropertyChanges {
                        target: datapane
                        height : parent.height - modellertools.height + 10
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
            height : 170
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.right: parent.right
        }


    }

}
