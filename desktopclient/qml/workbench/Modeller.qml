import QtQuick 2.0

Rectangle {
    id : modellerContainer
    property int defaultWidth: defaultFunctionBarWidth

    signal unloadcontent(string content)

    color : background4
    clip : true
    width : defaultWidth
    state : "invisible"
    height : 500
    opacity : 1

    FunctionBarHeader{
        headerImage: "../images/modellerCS1.png"
        headerText:"Modeller"
    }








    states: [
        State { name: "visible"

            PropertyChanges {
                target: modellerContainer
                opacity : 1
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: modellerContainer
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
                    unloadcontent("Modeller.qml")
                }
            }

        }
    ]
}

