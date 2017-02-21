import QtQuick 2.0
Item {
    id: root
    width: 1
    height: 1

    signal dragged()

    property bool active : true
    property bool fixedXX : false

    Rectangle {
        anchors.centerIn: parent
        width: 5
        height: 5
        border.color: "green"
        border.width: 1
        opacity: active ? 1.0 : 0.3
        color: "transparent"

        MouseArea {
            anchors.centerIn: parent
            width: parent.width + 5
            height: parent.height + 5
            drag.target: root
            drag.axis: fixedXX ? Drag.YAxis : Drag.XAndYAxis

            onPositionChanged: {
                if(drag.active) {
                    dragged()
                }
            }
        }
    }
}
