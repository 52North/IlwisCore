import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import "../.." as Base
import "../../workbench" as Workbench
import "../../Global.js" as Global
import "../../controls" as Controls
//import "../../datapanel" as DataPane

Rectangle {
    id : container




    clip : true
    state : "invisible"
    opacity : 0

    signal unloadcontent(string content)

//    CreateWorkflow{
//        anchors.top : functionBarHeader.bottom
//        anchors.topMargin: 5
//        width : parent.width
//        height : 210
//    }

    Workbench.FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/modellerCS1.png"
        headerText:qsTr("Modeller")
    }



    states: [
        // This adds a second state to the container where the rectangle is farther to the right

        State { name: "visible"

            PropertyChanges {
                target: container
                opacity : 1
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: container
                opacity : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "opacity"; duration : 500 ; easing.type: Easing.Linear }
            onRunningChanged :
            {
                if ( opacity == 0) {
                    unloadcontent("ModellerSelection.qml")
                }
            }
        }


    ]

}
