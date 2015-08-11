import QtQuick 2.0

Rectangle {
    id : container
    property int currentColor : 0
    signal colorChanged;

    function changeColor(){
        colorChanged()
    }

    property var model
    ListView {
        id : listview
        anchors.fill: parent
        model : container.model
        delegate : Rectangle{
            height : 10
            width : parent.width
            color : modelData
            border.width: 1
            border.color: "grey"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    currentColor = index
                    container.state = "invisible"
                    changeColor()
                }
            }

        }
        clip : true
    }
    states: [
        State { name: "visible"

            PropertyChanges {
                target: container
                opacity : 1
                enabled : true
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: container
                opacity : 0
                enabled : false
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "opacity"; duration : 500 ; easing.type: Easing.InOutCubic }
        }
    ]
}

