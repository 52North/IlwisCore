import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UserMessageHandler 1.0
import MessageModel 1.0
import "../Global.js" as Global
import ".." as Base

WorkBenchShifter {
    id : messageContainer

    clip : true
    state : "invisible"
    height : parent.height

    FunctionBarHeader{
        id : functionBar
        headerImage: "../images/message.png"
        headerText:"Messages"
    }

    TabView{
        width : parent.width
        height : parent.height - functionBar.height
        anchors.top: functionBar.bottom
        anchors.topMargin: 3

        style: Base.TabStyle1{}
        Tab {
            title : qsTr("Log")
            Item {
                width : parent.width
                height : parent.height
                Item{
                    id : clearArea
                    width : parent.width
                    height : 24
                    Button{
                        id : clearButton
                        text : "Clear error/warning status"
                        anchors.verticalCenter: parent.verticalCenter
                        height : 20
                        width: parent.width - 20
                        anchors.horizontalCenter: parent.horizontalCenter
                        onClicked: {
                            messagehandler.resetColor(-1)
                        }

                    }
                }

                Item {
                    id : messages
                    anchors.top : clearArea.bottom;
                    height : parent.height - clearArea.height - 10
                    width : parent.width
                    TableView {
                        id : messageTable
                        anchors.fill: parent
                        model : messagehandler.messages
                        selectionMode : SelectionMode.SingleSelection


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
                                TextInput {
                                    id : textmessage
                                    anchors.fill: parent
                                    text : styleData.value
                                    font.pointSize: 8
                                    readOnly: true;
                                    clip : true
                                    color : model[styleData.row].color
                                    wrapMode: Text.WordWrap
                                    MouseArea{
                                        anchors.fill: parent
                                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                                        onClicked: {

                                           messageTable.selection.forEach( function(rowIndex) {messageTable.selection.deselect(rowIndex)} )
                                           messageTable.selection.select(styleData.row)
                                           textmessage.selectAll()
                                           textmessage.copy()
                                           textmessage.deselect()

                                            if ( mouse.button & Qt.RightButton){
                                                menu.popup();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        rowDelegate: Rectangle {
                            height : 30
                            color : styleData.selected ? Global.selectedColor : (styleData.alternate? uicontext.lightestColor: "white")

                        }
                    }
                }
            }
        }

        Tab {
            title : qsTr("Commands")
            Item {
                id : commands
                width : parent.width
                height : parent.height
                TableView {
                    id : commandsTable
                    anchors.fill: parent
                    model : messagehandler.commands
                    selectionMode : SelectionMode.SingleSelection


                    TableViewColumn{
                        id : timeColumn
                        role : "shorttime"
                        title : "Time"
                        width : 60
                        delegate : Item{
                            id : shorttimeitemcommand
                            width : parent.width
                            Text {
                                text : styleData.value
                                font.pointSize: 8
                                color : model[styleData.row].color
                            }
                        }
                    }
                    TableViewColumn{
                        id : commandColumn
                        role : "message"
                        title : "Commands"
                        width : commandsTable.width - 60
                        delegate : Item{
                            id : commandItem
                            width : parent.width
                            TextInput {
                                id :commandmessage
                                anchors.fill: parent
                                text : styleData.value
                                font.pointSize: 8
                                readOnly: true;
                                clip : true
                                color : model[styleData.row].color
                                wrapMode: Text.WordWrap
                                MouseArea{
                                    anchors.fill: parent
                                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                                    onClicked: {
                                        commandsTable.selection.forEach( function(rowIndex) {commandsTable.selection.deselect(rowIndex)} )
                                        commandsTable.selection.select(styleData.row)
                                        commandmessage.selectAll()
                                        commandmessage.copy()
                                        commandmessage.deselect()
                                        if ( mouse.button & Qt.RightButton){
                                            menu.popup();
                                        }
                                    }
                                }
                            }
                        }
                    }
                    rowDelegate: Rectangle {
                        height : 30
                        color : styleData.selected ? Global.selectedColor : (styleData.alternate? uicontext.lightestColor: "white")

                    }
                }
            }
        }

        Tab {
            title : qsTr("results")
            Item {
                anchors.fill: parent
                Item{
                    id : clearResults
                    width : parent.width
                    height : 24
                    Button{
                        id : clearResultButton
                        text : qsTr("Clear results")
                        anchors.verticalCenter: parent.verticalCenter
                        height : 20
                        width: parent.width - 20
                        anchors.horizontalCenter: parent.horizontalCenter
                        onClicked: {
                            messagehandler.clearResults()
                        }

                    }
                }
                TextArea {
                    width : parent.width
                    height : parent.height - clearResults.height
                    anchors.top : clearResults.bottom
                    anchors.topMargin: 2
                    readOnly: true
                    text : messagehandler.results
                }
            }
        }
    }
    Menu {
          id: menu
          MenuItem {
              text: "copy"
              onTriggered: {

              }
          }
      }
}
