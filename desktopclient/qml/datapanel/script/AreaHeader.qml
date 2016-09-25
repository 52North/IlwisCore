import QtQuick 2.0

BorderImage {
    property string labeltext

    source: "../../images/tab_thin_active.png"
    width: parent.height - 7
    height :33
    border.left: 5; border.top: 5
    border.right: 5; border.bottom: 5
    rotation: 270
    z : 1
    y : parent.height / 2 - height/2
    x : -width / 2 + height/2 + 2
    Text {
        anchors.centerIn: parent
        text : qsTr(labeltext)
        font.pointSize: 11
    }
}
