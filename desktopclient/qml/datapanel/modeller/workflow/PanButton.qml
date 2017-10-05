import QtQuick 2.0
import "../../../Global.js" as Global

Rectangle {
    id : panButCont
    property var direction
    property var rotate
    border.width: 1
    border.color: Global.edgecolor
    opacity : 0.7

    onDirectionChanged: {
        if ( direction === "right")
            rotate = 0
        if ( direction === "left")
            rotate = 180
        if ( direction === "up")
            rotate = -90
        if ( direction === "down")
            rotate = 90

    }

    MouseArea {
        id : bb
        height : buttonSize
        width : buttonSize
        property int count : 1
        onReleased: {
           bb.count = 1
           panButCont.opacity = 0.7
        }

        Timer {
            id:  pressAndHoldTimer
            interval: 100
            running: bb.pressed
            repeat: true
            onTriggered: {
                if ( bb.pressed ){
                    panButCont.opacity = 1
                    var shift = Math.min(50,Math.max(1,bb.count))
                    workarea.pan2(shift,direction)
                    bb.count = bb.count + 1
                }
            }
        }

        Image {
            opacity: 0.6
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            rotation: rotate

            source : iconsource("arrowright.png")
        }
    }
}
