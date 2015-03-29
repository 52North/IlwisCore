import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "workbench" as WorkBench
import "datapanel" as DataPanel
import "controls" as Control
import MessageModel 1.0
import ResourceModel 1.0
import "Global.js" as Global

ApplicationWindow {
    id : bigthing
    width: 1400
    height : 1000
    color : "light grey"

    property int maxPull : 500


    property color background1 : "#EAECEE"
    property color background2 : "#DBDFE3"
    property color background3 : "#FFFDF4"
    property color background4 : "white"
    property int defaultFunctionBarWidth : 350
    property int activeCatalog: 0

    menuBar : MenuBar {
        id : mainMenu
        Menu {
            title: "Dummy 2"
            MenuItem { text: "Open..." }
            MenuItem { text: "Close" }
        }

        Menu {
            title: "Dummy 1"
            MenuItem { text: "Cut" }
            MenuItem { text: "Copy" }
            MenuItem { text: "Paste" }
        }
    }

    function addCatalog(){

        mainSplit.newCatalog("",-1)
    }

    function changeCatalog(url){
        mainSplit.changeCatalog(url)
    }

    function transitionInfoPane(newpagename) {
        workBench.transitionInfoPane(newpagename)
    }

    function unloadcontent(newpagename) {
        workBench.unloadcontent(newpagename)
    }

    function modellerPane(name) {
        if (dataPanel.addNewTab(name)) {
            dataPanel.showModellerPane(name)
        } else {
            removeModellerPane(name)
        }
    }

    function showModellerPane(name) {
        dataPanel.showModellerPane(name)
    }

    function removeModellerPane(name) {
        dataPanel.removeTab(name)
    }

    function updateSelectedItem(name) {
        var pane = workBench.currentPane()
        if (pane !== null && pane.toString().indexOf("Modeller") !== -1) {
            pane.updateSelectedItem(name)
        }
    }

    function getWorkbenchPane(name){
        if ( workBench.lastPage !== name)
            workBench.transitionInfoPane(name)
    }

    Rectangle {
        id : root
        anchors.fill : parent
        color : "#DDDDDD"

        Rectangle {
            id : commLine
            height : 35
            width : parent.width - 10
            color : "#DDDDDD"
            Row {
                id : workspace
                height : parent.height
                width : 250
                spacing: 10
                x : 5
                Text {
                    text : qsTr("Current Workspace")
                    anchors.verticalCenter: parent.verticalCenter
                }
                TextField{
                    width : 150
                    text : "default"
                    readOnly: true
                    height : 25
                    style: TextFieldStyle {
                        textColor: "black"
                        background: Rectangle {
                            anchors.fill: parent
                            color: Global.alternatecolor3
                        }
                    }
                    anchors.verticalCenter: parent.verticalCenter

                }
                anchors.verticalCenter: parent.verticalCenter
            }
            Control.CommandLine{
                anchors.left : workspace.right
                anchors.leftMargin: 10
                anchors.right: parent.right
                anchors.verticalCenter: workspace.verticalCenter
            }
        }

        SplitView {
            id : mainSplit
            orientation: Qt.Horizontal
            width: parent.width
            anchors.top : commLine.bottom
            height : bigthing.height - commLine.height * 2

            function addCatalog() {
                dataPanel.addCatalog()
            }

            function newCatalog(url, splitside){
                dataPanel.newCatalog(url, splitside)
            }

            function changeCatalog(url){
                dataPanel.changeCatalog(url)
            }

            WorkBench.WorkBenchButtonBar{
                id : workBenchButtons
            }

            WorkBench.WorkBench{
                id : workBench
                state : "invisible"
            }

            DataPanel.DataPane{
                id : dataPanel
            }

        }

    }


    statusBar: StatusBar {
        id : statusB
        RowLayout {
            Label { text: "Read Only" }
            Label { text: "More text" }
        }
    }
}
