import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../controls" as Controls

ToolButton{
    property string iconname
    id : nav
    property string label
    height : parent.width// buttonB.width
    width : parent.width
    onWidthChanged: {
        if ( width > 75){
            textLabel.width = nav.width
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
            color : control.pressed ? "#B0C4DE" : "white"
            border.width: 1
            border.color: "lightgrey"
        }
    }
}
