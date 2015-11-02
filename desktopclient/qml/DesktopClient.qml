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
    height : Screen.desktopAvailableHeight * 0.8
    color : "light grey"

    onClosing: {
        uicontext.exitUI()
    }

    property int maxPull : 500
    property int defaultFunctionBarWidth : bigthing.width * 0.25
    property int activeCatalog: 0

    function newCatalog(filter, outputtype, url, side){

         mainSplit.newCatalog(filter, outputtype, url, side)
     }

     function changeCatalog(filter, outputtype, url){
         mainSplit.changeCatalog(filter, outputtype, url)
     }

     function transitionInfoPane(newpagename, show) {
         workBench.transitionInfoPane(newpagename, show)
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
        if ( workBench.lastPage !== name){
            workBench.transitionInfoPane(name, "visible")
            workBenchButtons.setButtonStatus(name,"on")
        }
    }

    Rectangle {
        id : root
        y : 0
        height : parent.height
        width : parent.width
        color : Global.alternatecolor5

        Rectangle {
            id : commLine
            height : textArea.height
            width : parent.width - 10
            color : Global.alternatecolor5
            Row {
                id : workspace
                height : parent.height
                width : 250
                spacing: 10
                x : 5
                Text {
                    y : 6
                    text : qsTr("Current Workspace")
                }
                ComboBox{
                    width : 150
                    height : 25
                    objectName: "workspace_combobox_mainui"
                    model : mastercatalog.workspaces
                    textRole: "displayName"
                    y : 2
                    onCurrentIndexChanged: {
                        if ( currentIndex >= 0){
                            var wmodel =model[currentIndex]
                            if ( wmodel){
                                var filter = "resource='" + wmodel.url + "'"
                                dataPanel.changeCatalog(filter,"catalog",wmodel.url)
                                uicontext.currentWorkSpace = wmodel
                            }
                        }
                    }

                }
            }
            Control.CommandLine{
                id : textArea
                y : 2
                anchors.left : workspace.right
                anchors.leftMargin: 2
                anchors.right: parent.right
            }
        }

        SplitView {
            id : mainSplit
            orientation: Qt.Horizontal
            width: parent.width
            anchors.top : commLine.bottom
            anchors.bottom : parent.bottom

            function changeWidthSplitter(pside, partside){
                if ( pside === 1 && partside === 0){
                    if ( workBench.width === 0){
                        workBenchButtons.state = "invisible"
                    }else
                        workBench.state = "invisible"
                }
                if ( pside === 1 && partside === 1){
                    if ( workBenchButtons.width === 0 && workBench.width == 0){
                        workBenchButtons.state = "visible"
                    } else
                        workBench.state = "visible"
                }
                if ( pside === 0 && partside === 0){
                    workBenchButtons.state = "invisible"
                }
                if ( pside === 0 && partside === 1){
                    workBenchButtons.state = "visible"
                }
            }

            handleDelegate: Controls.SplitHandle{
                imageHeight: 22
                func : mainSplit.changeWidthSplitter
            }

            function newCatalog(filter, outputtype, url,side){
                dataPanel.newCatalog(filter, outputtype, url,side)
            }

            function changeCatalog(filter, outputtype, url){
                dataPanel.changeCatalog(filter, outputtype, url)
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
}
