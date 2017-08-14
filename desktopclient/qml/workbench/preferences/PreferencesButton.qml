import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global


Button{
    width : parent.width
    height : 29
    property var connectedItem
    property alias imageSource : img.source
    property alias labelText : label.text
    Row{
        x : 3
        spacing : 8
        width : parent.width - 3
        height : parent.height - 2
        anchors.verticalCenter:  parent.verticalCenter
        Image {
            id : img
            x : 7
            width : 21
            height : 21
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter:  parent.verticalCenter
        }
        Text {
            id : label
            height : 20
            anchors.verticalCenter:  parent.verticalCenter
            width : 50
        }
    }
    onClicked: {
        if ( connectedItem){
            connectedItem.state = connectedItem.state === "minimized" ? "maximized" : "minimized"
        }
    }
}

