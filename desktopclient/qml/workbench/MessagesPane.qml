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
                            color : styleData.selected ? "#B0B0B0" : (styleData.alternate? Global.lightestgreen: "white")
                        }
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
}
