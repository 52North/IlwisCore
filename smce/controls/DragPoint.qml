import QtQuick 2.0
Item {
    id: root

    signal dragged(int mx, int my)

    property bool active : true
    property bool fixedXX : false

    Rectangle {
        anchors.centerIn: parent
        width: 6
        height: 6
        border.color: "black"
        border.width: 2
        //color: "blue"
        opacity: active ? 1.0 : 0.3

        MouseArea {
            anchors.fill: parent
            drag.target: root
            drag.axis: fixedXX ? Drag.YAxis : Drag.XAndYAxis

            drag.minimumX : 10
            drag.maximumX : 410
            drag.minimumY : 0
            drag.maximumY : 400

            onPositionChanged: {
                if(drag.active) {
                    dragged(mouse.x, mouse.y)
                }
            }
        }
    }
}
