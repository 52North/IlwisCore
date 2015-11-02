import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UserMessageHandler 1.0
import MessageModel 1.0
import "../Global.js" as Global

WorkBenchShifter {
    id : messageContainer

    clip : true
    state : "invisible"
    height : parent.height

    FunctionBarHeader{
        id : functionBar
        headerImage: "../images/message40.png"
        headerText:"Messages"
    }

    Item{
        id : clearArea
        width : parent.width
        height : 24
        anchors.top : functionBar.bottom
        Button{
            id : clearButton
            text : "Clear error/warning status"
            anchors.verticalCenter: parent.verticalCenter
            height : 20
            width: parent.width - 20
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                messagehandler.resetColor(0)
            }

        }
    }

    Item {
        id : messages
        anchors.top : clearArea.bottom;
        height : parent.height - functionBar.height - clearArea.height - 10
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
}
