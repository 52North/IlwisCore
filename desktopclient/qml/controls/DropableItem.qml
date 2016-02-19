import QtQuick 2.0

Item {
    id : dropableItem
    width: parent.width
    height: 0
    visible: false
    property int maxHeight : 0
    property int interval : 200

    states: [
        State { name: "visible"

            PropertyChanges {
                target: dropableItem
                height : maxHeight
                visible : true
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: dropableItem
                height : 0
                visible : false
            }
        },
        State {
            name : "show"
            PropertyChanges {
                target: dropableItem
                height : maxHeight
                visible : true
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "height"; duration : interval ; easing.type: Easing.InOutCubic }
        }
    ]
}


