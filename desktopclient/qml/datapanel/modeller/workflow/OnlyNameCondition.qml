import QtQuick 2.2

Rectangle {
    color: "transparent"
    anchors.fill: parent
    property variant modelData: testModel.get(index)
    Text {
        anchors.centerIn: parent
        text: modelData.name
        font.strikeout: modelData.set
        font.bold: !modelData.set
    }
}
