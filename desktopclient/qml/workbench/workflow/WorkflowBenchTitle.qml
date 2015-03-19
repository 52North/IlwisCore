import QtQuick 2.0

Rectangle {
    id: title

    anchors.margins: 15
    height: 24

    property var titleText

    Text {
        text: titleText
        font.family: "Arial"
        font.bold: true
        font.pixelSize: 16
    }

    WorkflowSeparator { }
}
