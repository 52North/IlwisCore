import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import WorkSpaceModel 1.0
import "../controls" as Controls
import "../Global.js" as Global
import ".." as Base


Item {
    id : container
    clip : true
    opacity : 0

    signal unloadcontent(string content)

    FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/helpCS1.png"
        headerText:qsTr("Information")
    }

    TabView{
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 10
        width : parent.width
        height : parent.height - functionBarHeader.height - 15
        Tab{
            title : qsTr("About")
            Item {
                anchors.fill: parent
                Column {
                    anchors.fill: parent
                    anchors.margins: 5
                    spacing : 10
                    Item {
                        x : parent.width / 4
                        width : 180
                        height:  90
                        Row{
                            width : parent.width
                            height : parent.height
                            spacing : 10
                            Image {
                                source : "../images/itclogobig.png"
                                smooth: false
                            }
                            Image {
                                source : "../images/ilwislogobig.png"
                                smooth : false
                            }
                        }
                    }
                    Text {
                        height : 30
                        text : "Ilwis 4.0 pre alpha"
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pointSize: 14
                        font.bold : true
                    }

                    Controls.TextAreaLabelPair{
                        width : parent.width
                        height: 100
                        labelText: qsTr("Contact")
                        labelWidth: 100
                    }

                    Controls.TextAreaLabelPair{
                        width : parent.width
                        height: 100
                        labelText: qsTr("Loaded modules")
                        labelWidth: 100
                    }

                    Controls.TextAreaLabelPair{
                        width : parent.width
                        height: 100
                        labelText: qsTr("Developers")
                        labelWidth: 100
                        content: "Me,\n myself\n and I"
                    }
                }
            }
        }
        Tab{
            title : qsTr("Help")
            anchors.fill: parent
        }
        style: Base.TabStyle1 {
            id : tabstyle
        }
    }

    states: [
        State { name: "visible"

            PropertyChanges {
                target: container
                opacity : 1
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: container
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
                    unloadcontent("Info.qml")
                }
            }

        }
    ]

}
