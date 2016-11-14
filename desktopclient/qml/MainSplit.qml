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


Rectangle {
    id : mainSplit

    function newCatalog(filter, outputtype, url,side){
         dataPanel.newCatalog(filter, outputtype, url,side)
    }

    function changeCatalog(filter, outputtype, url){
        dataPanel.changeCatalog(filter, outputtype, url)
    }

    function transitionInfoPane(newpane,visstate) {
        workBench.transitionInfoPane(newpane, visstate)
    }

    function getWorkbenchPane(name, vstate){
        if ( workBench.lastPage !== name){
            if ( vstate === "visible"){
                workBench.transitionInfoPane(name, "visible")
                workBenchButtons.setButtonStatus(name,"on")
            }
            if ( vstate === "invisible"){
                workBench.transitionInfoPane(name, "invisible")
                workBenchButtons.setButtonStatus(name,"off")
            }
            if ( vstate === "toggle"){
                workBench.transitionInfoPane(name, "toggle")
                workBenchButtons.setButtonStatus(name,"toggle")
            }
        }
    }
    WorkBench.WorkBenchButtonBar{
        id : workBenchButtons
    }

    SplitView {
        width : parent.width - workBenchButtons.width
        height : parent.height
        anchors.left: workBenchButtons.right
        orientation: Qt.Horizontal

        WorkBench.WorkBench{
            id : workBench
            datapanel: dataPanel
            state : "invisible"
        }

        DataPanel.DataPane{
            id : dataPanel
            width : parent.width - workBench.width
            anchors.left: workBench.right
        }
    }



}

