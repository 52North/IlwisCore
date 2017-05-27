import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import WorkSpaceModel 1.0
import StartIlwis 1.0
import "../controls" as Controls
import "../Global.js" as Global
import ".." as Base


WorkBenchShifter {
    id : infoContainer
    clip : true
    state : "invisible"

    FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/info.png"
        headerText:qsTr("Information")
    }

    TabView{
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 10
        width : parent.width
        height : parent.height - functionBarHeader.height - 15
        Tab{
            title : qsTr("About")
            Rectangle {
                anchors.fill: parent
                color :  uicontext.lightestColor

                Column {
                    anchors.fill: parent
                    anchors.margins: 5
                    spacing : 10
                    Item {
                        x : 0
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
                                source : "../images/52nlogobig.png"
                                smooth : false
                            }
                        }
                    }
                    Text {
                        height : 20
                        text : "Ilwis 4: " + startilwis.buildType() + " " + (startilwis.verionNumber() != "0" ? startilwis.verionNumber() : "")
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pointSize: 14
                        font.bold : true
                        width : parent.width
                        opacity: width < 10 ? 0 : 1

                        clip : true
                    }
                    Text {
                        height : 15
                        text : "Build number: " + startilwis.buildNumber()
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pointSize: 10
                        width : parent.width
                        opacity: width < 10 ? 0 : 1

                        clip : true
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
                        content : uicontext.moduleData()
                    }

                    Controls.TextAreaLabelPair{
                        width : parent.width
                        height: 100
                        labelText: qsTr("Developers")
                        labelWidth: 100
                        content: "Martin Schouwenburg(lead),\nHenning Bredel,\nBas Retsios,\nHeinrich Kiessling,\nJohannes Kolbe\nJoão Bonina\nWim Feringa(UI)"
                    }
                    Controls.TextAreaLabelPair{
                        width : parent.width
                        height: 60
                        labelText: qsTr("Documentation")
                        labelWidth: 100
                        content: "Jamshid Farifteh"
                    }
                }
            }
        }
        Tab{
            title : qsTr("Wiki")
            anchors.fill: parent

        }
        Tab {
            title : "debug"
            enabled: uicontext.debugMode
            visible: uicontext.debugMode
            DebugInfo{
                anchors.fill: parent
            }
        }

        style: Base.TabStyle1 {
            id : tabstyle
        }
    }
}
