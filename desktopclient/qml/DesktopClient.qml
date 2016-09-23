import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import QtQuick.Window 2.1
import "workbench" as WorkBench
import "datapanel" as DataPanel
import "controls" as Control
import MessageModel 1.0
import ResourceModel 1.0
import MasterCatalogModel 1.0
import QtGraphicalEffects 1.0
import "Global.js" as Global
import "controls" as Controls

ApplicationWindow {
    id : bigthing
    width: Screen.width * 0.8
    height : Screen.height * 0.9
    color : Global.lightgreen
    objectName: "mainwindow__mainui"

    onClosing: {
        uicontext.exitUI()
        rootwindow.close()
    }

    property int maxPull : 500* Global.uiScale
    property int defaultFunctionBarWidth : Math.min(bigthing.width * 0.25 * Global.uiScale,390 * Global.uiScale)
    property int activeCatalog: 0

    function newCatalog(filter, outputtype, url, side){

        mainSplit.newCatalog(filter, outputtype, url, side)
    }

    function changeCatalog(filter, outputtype, url){
        mainSplit.changeCatalog(filter, outputtype, url)
    }

    function transitionInfoPane(newpagename, show) {
        mainSplit.transitionInfoPane(newpagename, show)
    }


    function getWorkbenchPane(name, vstate){
        mainSplit.getWorkbenchPane(name, vstate)
    }

    Controls.ToolTipLabel{id : toolTip}

    SplitView {
        id : root
        height : parent.height
        width : parent.width
        orientation: Qt.Vertical
        property int defaultCommLineHeight : 60

        Item {
            id : topArea
            width : parent.width
            height : root.height - mainSplit.height



            Rectangle {
                anchors.left: parent.left
                anchors.top : parent.top
                width : xu1.width + 1
                height : 100
                color : Global.palegreen
                border.width: 1
                border.color : "#b3b3b3"
            }

            Item {
                id : xu1
                anchors.left: parent.left
                anchors.top : parent.top
                width : dummy.width + 7
                height : 100

                Rectangle{
                    id : dummy
                    height : 100
                    width : 70 * Global.uiScale
                    anchors.left: parent.left
                    anchors.top : parent.top
                    color : Global.middlegreen
                }
            }
            DropShadow {
                id: butShadow2
                anchors.fill: source
                cached: true
                horizontalOffset: 4
                radius: 6
                samples: 12
                color: "#80000000"
                smooth: true
                opacity: 0.7
                source: xu1
            }
            CommandLineArea {
                id : commLine
                width : parent.width
                height: parent.height
            }
            states: [
                State {
                    name: "collapsed"
                    PropertyChanges {
                        target: topArea
                        height : root.defaultCommLineHeight
                    }

                },
                State{
                    name: "open"
                    PropertyChanges {
                        target: topArea
                        height: 250
                    }
                }

            ]

            transitions: [
                Transition {
                    NumberAnimation { target: topArea; property: "height"; duration: 300 }
                }
            ]
        }


        MainSplit {
            id : mainSplit
            width: parent.width
            height : root.height - root.defaultCommLineHeight
            Layout.fillHeight: true
        }

    }
}
