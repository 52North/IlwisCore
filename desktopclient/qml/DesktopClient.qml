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
import "Global.js" as Global
import "controls" as Controls

ApplicationWindow {
    id : bigthing
    width: Screen.desktopAvailableWidth * 0.8
    height : Screen.desktopAvailableHeight * 0.9
    color : "light grey"
    objectName: "mainwindow__mainui"

    onClosing: {
        uicontext.exitUI()
        rootwindow.close()
    }

    property int maxPull : 500
    property int defaultFunctionBarWidth : Math.min(bigthing.width * 0.25,390)
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


    function getWorkbenchPane(name){
        mainSplit.getWorkbenchPane(name)
    }

    Rectangle {
        id : root
        y : 0
        height : parent.height
        width : parent.width
        color : Global.alternatecolor5

        CommandLineArea {
            id : commLine
            width : parent.width - 10

        }

        MainSplit {
            id : mainSplit
            orientation: Qt.Horizontal
            width: parent.width
            anchors.top : commLine.bottom
            anchors.bottom : parent.bottom
        }

    }
}
