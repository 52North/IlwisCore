import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../controls" as Controls

ToolButton{
    id : button
    property string label
    height :25

    width : parent.width / 5
    clip : true

    Text {
        id : textLabel
        width : contentWidth
        height : 15
        text : label
        visible : true
        font.weight: Font.DemiBold
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }

    style : ButtonStyle{
        background: Rectangle {
            anchors.fill: parent
            color : control.pressed ? "#B0C4DE" : (side == "left") ? "white" : "#F6F6F6"
            border.width: 1
            border.color: "lightgrey"
        }
    }
    states: [
        State { name: "fullsize"

            PropertyChanges {
                target: button
                width : (parent.width) / 5
                opacity : 1
                enabled : true
            }
        },
        State {
            name : "zerosize"
            PropertyChanges {
                target: button
                height : 0
                opacity : 0
                enabled : false
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "width"; duration : 500 ; easing.type: Easing.InOutCubic }
            NumberAnimation { properties: "opacity"; duration : 500 ; easing.type: Easing.InOutCubic }
        }
    ]

    Component.onCompleted: {
        button.state = "fullsize"
    }

}

