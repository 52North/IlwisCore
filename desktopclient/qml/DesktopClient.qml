import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "workbench" as WorkBench
import "datapanel" as DataPanel
import "controls" as Control
import MessageModel 1.0
import ResourceModel 1.0

ApplicationWindow {
    id : bigthing
    width: 1300
    height : 900
    color : "light grey"

    property int maxPull : 500


    property color background1 : "#EAECEE"
    property color background2 : "#DBDFE3"
    property color background3 : "#FFFDF4"
    property color background4 : "#EEEEEE"
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


    function transitionInfoPane(newpagename) {
        workBench.transitionInfoPane(newpagename)
    }

    function unloadcontent(newpagename) {
        workBench.unloadcontent(newpagename)
    }
    Rectangle {
        id : root
        anchors.fill : parent

        Control.CommandLine{
            id : commLine
        }

        SplitView {
            id : mainSplit
            orientation: Qt.Horizontal
            width: parent.width
            anchors.top : commLine.bottom
            height : bigthing.height - commLine.height * 2

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

            Component.onCompleted: {
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
