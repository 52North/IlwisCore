import QtQuick 2.0

Rectangle {
    id : workarea
    width : parent.width
    height : 0
    states: [
        State { name: "visible"

            PropertyChanges {
                target: workarea
                height : parent.height
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: workarea
                height : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "height"; duration : 500 ; easing.type: Easing.InOutCubic }
        }
    ]

}

