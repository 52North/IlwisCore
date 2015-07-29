import QtQuick 2.0
import QtQuick.Controls 1.0
import "../../Global.js" as Global

Rectangle {

    anchors.margins : 5
    color : Global.alternatecolor1
    height: 28

    // define onFilterChanged Connection element
    signal filterChanged()

    Text {
        id : label
        text : qsTr("Filter")
        verticalAlignment: Text.AlignVCenter
        clip : false
        height : 20
        width: 50
        x : parent.x + 5
        y : 4
    }
    TextField {
        id : filter
        height: 20
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.left: label.right
        anchors.leftMargin: 5
        onTextChanged: filterChanged()
        y : 4
    }

    WorkflowSeparator {}
}
