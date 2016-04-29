import QtQuick 2.0

Rectangle{
    id : first
    width : bigthing.width / 6
    height : bigthing.height /6
    property int endX
    property int endY
    color : "red"
    rotation : 270
    opacity: 0
    property int easingType : Easing.InOutCubic
    property int time : 2000


    states : [
        State {
            name : "end"
            PropertyChanges {target: first ; x : endX }
            PropertyChanges {target: first ; y : endY}
            PropertyChanges {target: first ; rotation : 0}
            PropertyChanges {target: first ; opacity : 1}

        },
        State {
            name : "begin"
            PropertyChanges {target: first ; x : endX }
            PropertyChanges {target: first ; y : endY}
        }
    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "x"; duration : time ; easing.type: easingType }
            NumberAnimation { properties: "y"; duration : time ; easing.type: easingType }
            NumberAnimation { properties: "rotation"; duration : time ; easing.type: easingType}
            NumberAnimation { properties: "opacity"; duration : time ; easing.type: easingType}
        }
    ]
}

