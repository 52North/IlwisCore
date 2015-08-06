import QtQuick 2.0

Item {
    id : dropableItem
    width: parent.width
    height: 0
    property int maxHeight : 0

    states: [
        State { name: "visible"

            PropertyChanges {
                target: dropableItem
                height : maxHeight
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: dropableItem
                height : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "height"; duration : 400 ; easing.type: Easing.InOutCubic }
        }
    ]
}


