import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import Qt.labs.folderlistmodel 2.1
import "../../Global.js" as Global
import "../../controls" as Controls

Rectangle {
    id : flowParameterForm
    width: 350
    height: 0
    enabled: false
    anchors.centerIn: parent
    color : "white"
    state : "invisible"
    border.width: 1
    border.color: Global.edgecolor
    radius : 4
    z : 1
    clip : true

    Rectangle {
        id : header
        width : parent.width
        height : 20

        color : Global.headerlight
        Text {
            x : 5
            y : 3
            width : 100
            height : 18
            text : qsTr("Connect parameters")
            font.bold: true
        }
        radius : 3
    }

    Item {
        id : inParams
        anchors.top : header.bottom
        anchors.topMargin: 4
        x : 5
        width : parent.width
        height : 20
        Text {
            id : label
            height : 20
            width : 120
            text : qsTr("Outgoing parameters")
            font.pointSize: 8
            y :3
        }

        ComboBox{
            id : textid
            anchors.left : label.right
            height : 20
            width : parent.width - label.width -10
        }
    }
    Item {
        id : outParams
        anchors.top : inParams.bottom
        anchors.topMargin: 4
        x : 5
        width : parent.width
        height : 20
        Text {
            id : labelOut
            height : 20
            width : 120
            text : qsTr("Input parameters")
            font.pointSize: 8
            y :3
        }

        ComboBox{
            id : textid2
            anchors.left : labelOut.right
            height : 20
            width : parent.width - labelOut.width -10
        }
    }
    Button{
        text : qsTr("Apply")
        height : 20
        width : 60
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.top : outParams.bottom
        anchors.topMargin: 5
        onClicked: {
            flowParameterForm.state = "invisible"
        }
    }


    states: [
        State { name: "visible"

            PropertyChanges {
                target: flowParameterForm
                enabled : true
                height : 120
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: flowParameterForm
                enabled : false
                height : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation {
                properties: "height"; duration : 200 ; easing.type: Easing.Linear
            }

        }
    ]
}

