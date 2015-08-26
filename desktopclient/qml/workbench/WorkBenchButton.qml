import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../controls" as Controls
import "../Global.js" as Global

ToolButton{
    property string iconname
    id : button
    property string label
    height : parent.width// buttonB.width
    width : parent.width
    checkable: true
    checked: false
    onWidthChanged: {
        if ( width > 75){
            textLabel.width = button.width
            textLabel.visible = true
        }else
            textLabel.visible = false
    }

    Image { id : but
        anchors.centerIn: parent
        source: "../images/" + iconname
        fillMode: Image.PreserveAspectFit
    }
    Text {
        id : textLabel
        anchors.top : but.bottom
        width : 0
        height : 15
        text : label
        horizontalAlignment: Text.AlignHCenter
        visible : true
        font.weight: Font.DemiBold

    }

    style : ButtonStyle{
        background: Rectangle {
            anchors.fill: parent
            color : (control.pressed || checked) ? Global.selectedColor : "white"
            border.width: 1
            border.color: "lightgrey"
        }
    }
    states: [
        State { name: "fullsize"

            PropertyChanges {
                target: button
                height : (parent.height) / 9
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
            NumberAnimation { properties: "height"; duration : 500 ; easing.type: Easing.InOutCubic }
            NumberAnimation { properties: "opacity"; duration : 500 ; easing.type: Easing.InOutCubic }
        }
    ]

    Component.onCompleted: {
        button.state = "fullsize"
    }

}
