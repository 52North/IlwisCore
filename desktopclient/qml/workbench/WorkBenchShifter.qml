import QtQuick 2.0

Item {
    id : container
    height : parent.height



    states: [
        State { name: "visible"

            PropertyChanges {
                target: container
                width : defaultFunctionBarWidth
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: container
                width : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation {
                properties: "width"; duration : 500 ; easing.type: Easing.Linear
            }

        }
    ]
}

