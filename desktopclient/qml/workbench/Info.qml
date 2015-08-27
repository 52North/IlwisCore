import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import WorkSpaceModel 1.0
import "../controls" as Controls
import "../Global.js" as Global


Item {
    id : container
    clip : true
    opacity : 0

    signal unloadcontent(string content)

    FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/helpCS1.png"
        headerText:qsTr("Information")
    }

    states: [
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
            NumberAnimation {
                properties: "opacity"; duration : 500 ; easing.type: Easing.Linear
            }
            onRunningChanged :
            {
                if ( opacity == 0) {
                    unloadcontent("Info.qml")
                }
            }

        }
    ]

}
