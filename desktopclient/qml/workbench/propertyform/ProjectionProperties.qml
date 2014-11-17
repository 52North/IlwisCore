import QtQuick 2.0

Item {
    Text { id : line1; text : qsTr("Projection"); width: 120; font.bold: true }
    Text { text : projectionInfo;  height : 22;width: parent.width - line1.width - 2; anchors.left: line1.right}
}
