import QtQuick 2.0
Item {
    id: anchor
    width: 1
    height: 1

    Rectangle {
        anchors.centerIn: parent
        width: 5
        height: 5
        border.color: "green"
        border.width: 1
        color: "transparent"
    }
}
