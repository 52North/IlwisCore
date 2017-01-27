import QtQuick 2.0

Item {
    id : dropableItem
    width: parent.width
    height: 0
    visible: false
    enabled : height > 0
    property int maxHeight : 0
    property int interval : 200

    function toggle() {
        dropableItem.state = dropableItem.state == "invisible" ? "visible" : "invisible"
    }

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
            NumberAnimation { properties: "height"; duration : 300 ; easing.type: Easing.InOutCubic }
        }
    ]
}


