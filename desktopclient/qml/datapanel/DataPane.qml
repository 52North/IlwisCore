import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MessageModel 1.0
import ResourceModel 1.0
import UIContextModel 1.0
import MasterCatalogModel 1.0
import DataPaneModel 1.0
import ".." as Base
import "catalog" as Catalog
import "modeller" as Modeller
import "../workbench" as Workbench
import "../Global.js" as Global
import "../controls" as Controls

Rectangle {

    id: centerItem
    color: Global.alternatecolor5
    objectName : "datapane_container_mainui"
    height : parent.height - 16
    width : bigthing.width - workBenchButtons.width - workBench.width - 5
    property int activeSplit : 2

    /*
     * Signal, thrown if a tab is closed
     */
    signal closedTab(string title)
    function iconSource(name) {
        if ( name.indexOf("/") !== -1)
            return name

        var iconP = "../images/" + name
        return iconP

    }
    function getCurrentCatalogTab(){
        var tabview = Math.abs(activeSplit) == 1 ? lefttab : righttab
        if ( tabview && tabview.currentIndex >= 0 && tabview.count > 0) {
            var tab = tabview.getTab(tabview.currentIndex)
            if ( tab && tab.item){
                if ( "currentCatalog" in tab.item)
                    return tab.item
                else{ // apparently the tab has no catalog so we look at the other side
                    tabview = Math.abs(activeSplit) == 2 ? lefttab : righttab
                    if ( tabview && tabview.currentIndex >= 0 && tabview.count > 0) {
                        tab = tabview.getTab(tabview.currentIndex)
                        if ( tab && tab.item){
                            if ( "currentCatalog" in tab.item){
                                activeSplit = Math.abs(activeSplit) == 2 ? 1 : 2
                                return tab.item
                            }
                        }
                    }
                }
            }
        }
    }

    function newCatalog(filter,outputtype, url){
        datapanesplit.newPanel(filter, outputtype, url)
    }

    function setCatalogByIndex(currentTab, tabindex){
        currentTab.currentIndex = tabindex
        var catalogtab = getCurrentCatalogTab()
        if ( catalogtab && catalogtab.currentCatalog){
            mastercatalog.currentUrl = catalogtab.currentCatalog.url
            mastercatalog.currentCatalog = catalogtab.currentCatalog
        }
    }

    function changeCatalog(filter, outputtype, url){
        datapanesplit.changePanel(filter,outputtype, url)
    }

//    Loader {
//        id : mapWindow
//    }

    SplitView {
        id : datapanesplit
        objectName : "datapane_mainui"
        orientation: Qt.Horizontal
        anchors.fill: parent
        anchors.leftMargin: 3
//        property int tel: 0

        function closeTab(isleft, tabindex){
            var ok = datapane.removeTab(isleft, tabindex)
            if ( ok){
                var tabview = isleft ? lefttab : righttab
                tabview.removeTab(tabindex)

                if ( datapane.leftSide.tabCount === 0){
                    lefttab.state = "zerosize"
                    righttab.state = "fullsize"
                }else if(datapane.rightSide.tabCount === 0){
                    lefttab.state = "fullsize"
                    righttab.state = "zerosize"
                }
            }
        }

        function changePanel(filter, outputtype, url){
            var sidePanel = datapane.activeSide
            var tabview = sidePanel.tabview
            if ( tabview){
                var removeIndex = tabview.currentIndex + 1
                var newPanel = sidePanel.createPanel(tabview.currentIndex,filter,outputtype, url)
                if ( newPanel){
                    var component = Qt.createComponent(newPanel.componentUrl)
                    mastercatalog.currentUrl = url
                    var data= newPanel.displayName
                    var insertetTab = tabview.insertTab(tabview.currentIndex, data, component)
                    if ( insertetTab){
                        insertetTab.item.addDataSource(filter, url, outputtype)
                        //console.debug("1")
                        insertetTab.item.tabmodel = newPanel
                        //console.debug("2")
                        tabview.removeTab(removeIndex)
                        //console.debug("3")
                        datapane.select(sidePanel.side === "left", tabview.currentIndex, true)
                        //console.debug("4")
                    }
                }
            }
        }

        function newPanel(filter,outputtype, url) {
            var allNew = datapane.leftSide.tabCount === 0 && datapane.rightSide.tabCount === 0
            var newPanel = datapane.createPanel(filter,outputtype, url)
            if ( !newPanel)
                return
            var component = Qt.createComponent(newPanel.componentUrl)
            var sidePanel = datapane.activeSide
            var tabview = sidePanel.tabview
            if ( tabview){
                var data= newPanel.displayName
                var tab = tabview.addTab(data, component)
                tab.active = true
                tab.item.addDataSource(filter, url, outputtype)
                tab.item.tabmodel = newPanel
            }
            if ( allNew){
                lefttab.state = "fullsize"
            }else if ( datapane.leftSide.tabCount === 1 && datapane.rightSide.tabCount === 1){ // one of them was new, so halfsize
                lefttab.state = "halfsize"
                righttab.state = "halfsize"
            } //else we dont do anything, leave it as it is
            if ( sidePanel.side == "left"){
                lefttab.currentIndex = sidePanel.tabCount - 1
            }else{
                righttab.currentIndex = sidePanel.tabCount - 1
            }

        }

        function showTabInFloatingWindow(panelside, tabIndex) {
            var sidePanel = panelside === 1 ? datapane.leftSide : datapane.rightSide
            var tabview = sidePanel.tabview
            var tab = tabview.getTab(tabIndex)
            var tabData = sidePanel.tab(tabIndex)

            if (tab && tab.item) {
                var qml = "import QtQuick 2.1; import QtQuick.Window 2.1;"
                qml += "FloatingWindow { id: floatingWindow } ";
                var window = Qt.createQmlObject(qml, datapanesplit)
                window.height = tab.item.height
                window.width = tab.item.width
                window.show();
                window.datapanel =  tabData.componentUrl



                window.transfer(tab.item)

                closeTab(sidePanel.isLeft, tabIndex);
            }
        }

//        function addWorkflowCanvas(id, name) {
//            console.log("creating new workflow canvas")
//            var component = Qt.createComponent("workflow/WorkflowDataPane.qml")
////            var tabview = activeSplit === 1 ? righttab : lefttab
////            var tab = tabview.addTab(name, component)
////            tab.active = true
//            var tab = righttab.addTab(name,component)
//            tab.active = true
//            if ( activeSplit ===1){
//                righttab.width = parent.width / 2.0
//                activeSplit = 2
//            }
//            else{
//                lefttab.width = parent.width / 2.0
//                activeSplit = 1
//            }
//            tab.item.workflow = id;
//        }

//        function addModeller(name) {
//            var component = Qt.createComponent("modeller/ModellerPanel.qml")
//            var tab = activeSplit ===1 ? righttab.addTab(name,component) : lefttab.addTab(name,component)
//            tab.active = true
//            if ( activeSplit ===1){
//                righttab.width = parent.width / 2.0
//                activeSplit = 2
//            }
//            else{
//                lefttab.width = parent.width / 2.0
//                activeSplit = 1
//            }
//        }

        /*
         * Remove a tab by name from TabView
         */
//        function removeTabFromView(name) {
//            var ri = righttab.getTabIndexFor(name);
//            if (ri !== -1) {
//                closeTab(righttab.side, ri)
//            }
//            var li = lefttab.getTabIndexFor(name);
//            if (li !== -1) {
//                closeTab(lefttab.side, ri)
//            }
//        }


        function changeWidth(pside, partside){
            if ( partside === 0){
                lefttab.fillWidth = false
                lefttab.state = "zerosize"
            }else {
                lefttab.fillWidth = true
                righttab.state = "zerosize"

                activeSplit = 2            }
                activeSplit = 1
            }
        }
        function newModeller(name, splitside) {
            var component = Qt.createComponent("modeller/ModellerPanel.qml");
//            var tab = activeSplit ===1 ? righttab.addTab(name,component) : lefttab.addTab(name,component);
            var tab = righttab.addTab(name,component);
            tab.active = true;
             var tabCount = 0;
                if ( lefttab.count == 1){
                    righttab.state = "halfsize"
                }
                lefttab.state = righttab.count == 0 ? "fullsize" : "halfsize";
                activeSplit = 1;
                tabCount = lefttab.count - 1;
                lefttab.currentIndex = tabCount;
       }

        handleDelegate: Controls.SplitHandle{
            imageHeight: 22
            offset : 25
            handlePic: "splithandledark.png"
            func : datapanesplit.changeWidth
        }        DataTabView2 {
            id : lefttab
            property bool fillWidth : true
            side : 1
            objectName: "datapane_lefttab_mainui"
            Layout.fillWidth: fillWidth

            Component.onCompleted: {
                datapane.leftSide.setTabview(lefttab.objectName)
            }
        }


        DataTabView2 {
            id : righttab
            side : 2
            objectName: "datapane_righttab_mainui"
            Component.onCompleted: {
                datapane.rightSide.setTabview(righttab.objectName)
            }
        }

    }

}
