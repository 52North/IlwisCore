import QtQuick 2.1
import QtGraphicalEffects 1.0

Item {
    id :floatingrect
    property alias text: toolTip.text
    width : floatingrect.width
    height : floatrect.height
    opacity : 0
    enabled : false
    z: 1000

    Item {
        id: floatrect
        x : 5
        y : -15
        z: 1000
        width: content.width + (2*toolTipShadow.radius)
        height: content.height + (2*toolTipShadow.radius)

        Rectangle {
            id: content
            anchors.centerIn: parent
            width: toolTip.contentWidth + 8
            height: toolTip.contentHeight + 4
            radius: 3
            color : "cornsilk"

            Text {
                id: toolTip
                wrapMode: Text.WordWrap
                x : 4
                text : "abcdefghijklmnopqrstuvwxyz"
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    DropShadow {
        id: toolTipShadow
        z: 1000
        anchors.fill: source
        cached: true
        horizontalOffset: 2
        verticalOffset: 2
        radius: 8.0
        samples: 16
        color: "#80000000"
        smooth: true
        source: floatrect
        opacity : floatrect.opacity
    }
}

