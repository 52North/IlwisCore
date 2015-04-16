import QtQuick 2.0

Rectangle {

    property string titleText
    property string shortDescriptionText

    anchors.top : parent.top
    anchors.margins: 15

    // TODO childrenRect yields warnings: Binding loop detected for property "width"
    width: childrenRect.width + 20
    height: childrenRect.height + 20

    Text {
        id : text
        text: titleText
        font.family: "Arial"
        font.bold: true
        font.pixelSize: 16
    }

    TextInput {
        text: shortDescriptionText
        anchors.top : text.bottom
        font.family: "Arial"
        font.pixelSize: 10
        enabled: false
    }

    //WorkflowSeparator { }
}
