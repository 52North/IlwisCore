import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import WorkSpaceModel 1.0
import "../../Global.js" as Global

Column {
    width : parent.width
    height : parent.height / 5
    spacing: 3
    Text {
        id : title
        text : qsTr("Description")
        height: Global.rowHeight
        width : parent.width
        font.bold: true
    }
    Rectangle {
        width : parent.width
        height : parent.height  - title.height - 5
        color : Global.alternatecolor4
        TextArea {
            anchors.fill: parent
            anchors.margins: 2
            id : description
            backgroundVisible: false

        }
    }
}

