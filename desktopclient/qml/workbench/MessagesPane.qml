import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UserMessageHandler 1.0
import MessageModel 1.0

Rectangle {
    id : messageContainer

    signal unloadcontent(string content)

    color : background4
    clip : true
    state : "invisible"
    height : 500
    opacity : 1

    FunctionBarHeader{
        id : functionBar
        headerImage: "../images/message40.png"
        headerText:"Messages"
    }

    Rectangle {
        id : messages
        anchors.top : functionBar.bottom;
        height : parent.height - functionBar.height
        width : parent.width
        TableView {
            id : messageTable
            anchors.fill: parent
            model : messagehandler.messages


            TableViewColumn{
                id : imageColumn
                role : "shorttime"
                title : "Time"
                width : 60
                delegate : Item{
                    id : shorttimeitem
                    width : parent.width
                    Text {
                        text : styleData.value
                        font.pointSize: 8
                        color : model[styleData.row].color
                    }
                }
            }
            TableViewColumn{
                id : messageColumn
                role : "message"
                title : "Message"
                width : messageTable.width - 60
                delegate : Item{
                    id : messageitem
                    width : parent.width
                    Text {
                        anchors.fill: parent
                        text : styleData.value
                        font.pointSize: 8
                        color : model[styleData.row].color
                        wrapMode: Text.WordWrap
                        maximumLineCount : 2
                    }
                }
            }
            rowDelegate: Rectangle {
                height : 30
                color : styleData.selected ? "#B0B0B0" : (styleData.alternate? "#eee" : "#fff")
            }
        }
    }



    states: [
        State { name: "visible"

            PropertyChanges {
                target: messageContainer
                opacity : 1
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: messageContainer
                opacity : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation {
                properties: "opacity"; duration : 500 ; easing.type: Easing.Linear
            }
            onRunningChanged :
            {
                if ( opacity == 0) {
                    unloadcontent("MessagesPane.qml")
                }
            }

        }
    ]
}
