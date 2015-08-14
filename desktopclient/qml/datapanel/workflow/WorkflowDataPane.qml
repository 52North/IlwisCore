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

        WorkflowCanvas {
            id: canvas

            width : parent.width
            height : parent.height - 170
            state : "visible"
            states: [
                State { name: "visible"

                    PropertyChanges {
                        target: canvas
                        height : parent.height - modellertools.height - 170
                    }
                },
                State {
                    name : "invisible"
                    PropertyChanges {
                        target: canvas
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

        WorkflowManager{
            height : 170
        }


    }

}
