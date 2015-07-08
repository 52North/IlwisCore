import QtQuick 2.0

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UserMessageHandler 1.0
import MessageModel 1.0
import "./creators" as Create
import "../Global.js" as Global

Rectangle {
    id : creationContainer

    signal unloadcontent(string content)

    color : Global.mainbackgroundcolor
    clip : true
    state : "invisible"
    height : 500
    opacity : 1

    FunctionBarHeader{
        id : functionBar
        headerImage: "../images/createCS.png"
        headerText:"Object Creation"
    }

    Create.ObjectList{
        width : parent.width
        height : 200
        anchors.top : functionBar.bottom
        anchors.topMargin: 5
    }

    states: [
        State { name: "visible"

            PropertyChanges {
                target: creationContainer
                opacity : 1
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: creationContainer
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
                    unloadcontent("ObjectCreation.qml")
                }
            }

        }
    ]
}

