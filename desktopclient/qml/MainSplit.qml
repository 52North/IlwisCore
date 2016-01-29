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

SplitView {
    id : mainSplit

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

    function transitionInfoPane(newpane,visstate) {
        workBench.transitionInfoPane(newpane, visstate)
    }

    function getWorkbenchPane(name){
        if ( workBench.lastPage !== name){
            workBench.transitionInfoPane(name, "visible")
            workBenchButtons.setButtonStatus(name,"on")
        }
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

