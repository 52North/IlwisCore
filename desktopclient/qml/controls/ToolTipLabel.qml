import QtQuick 2.0
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0

Item {
    id: toolTipRoot
    width: toolTip.contentWidth
    height: toolTipContainer.height
    visible: false
    clip: false
    z: 999999999
    parent : root

    property alias text: toolTip.text
    property alias radius: content.radius
    property alias backgroundColor: content.color
    property alias textColor: toolTip.color
    property alias font: toolTip.font

    Item {
        id: toolTipContainer
        z: toolTipRoot.z + 1
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
                anchors.verticalCenter: parent.verticalCenter
            }
        }

    }

    DropShadow {
        id: toolTipShadow
        z: toolTipRoot.z + 1
        anchors.fill: source
        cached: true
        horizontalOffset: 2
        verticalOffset: 2
        radius: 8.0
        samples: 16
        color: "#80000000"
        smooth: true
        source: toolTipContainer
    }

    Behavior on visible { NumberAnimation { duration: 200 }}
}
