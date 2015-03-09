import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MessageModel 1.0
import ResourceModel 1.0
import UIContextModel 1.0
import MasterCatalogModel 1.0
import ".." as Base
import "catalog" as Catalog
import "modeller" as Modeller
import "../workbench" as Workbench




Rectangle {

    id: centerItem
    color: background4
    height : parent.height - 16
    width : bigthing.width - buttonB.width - infoP.width - 5
    property int activeSplit : 2

    function addModellerPanel(name) {
        mainsplit.addModeller(name)
    }

    function removeModellerPanel(name) {
        mainsplit.removeTabFromView(name);
    }

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
            }
        }
    }


    function newCatalog(url, splitside){
        if ( splitside !== -100){ // -100 is the magic number when starting the app for the first catalog when nothing has been set yet
            var catTab = getCurrentCatalogTab()
            if ( catTab){
                url = catTab.currentCatalog.url
                console.debug("url", url)
                splitside = activeSplit
            }
        }
        mainsplit.newCatalog(url,splitside)
    }

    function setCatalogByIndex(currentTab, tabindex){
        currentTab.currentIndex = tabindex
        var catalogtab = getCurrentCatalogTab()
        if ( catalogtab && catalogtab.currentCatalog){
            mastercatalog.currentUrl = catalogtab.currentCatalog.url
            mastercatalog.currentCatalog = catalogtab.currentCatalog
        }
    }

    function changeCatalog(url){
        mainsplit.changeCatalog(url)
    }

    Loader {
        id : mapWindow
    }

    SplitView {
        id : mainsplit
        orientation: Qt.Horizontal
        anchors.fill: parent
        property int tel: 0

        function closeTab(splitindex, tabindex1){
            if ( Math.abs(splitindex) === 1){ // left
                if ( righttab.count === 0 && lefttab.count === 1)
                    return
                lefttab.removeTab(tabindex1)
                if ( lefttab.count === 0){
                    lefttab.state = "zerosize"
                    righttab.state = "fullsize"
                    activeSplit = 2
                    setCatalogByIndex(righttab, 0)

                }else{
                    setCatalogByIndex(lefttab, tabindex1)
                }
            }
            else if ( Math.abs(splitindex) === 2){ // right
                if ( lefttab.count === 0 && righttab.count === 1)
                    return
                righttab.removeTab(tabindex1)
                if ( righttab.count === 0){
                    righttab.state = "zerosize"
                    lefttab.state = "fullsize"
                    activeSplit = 1
                    setCatalogByIndex(lefttab, 0)
                }else{
                    setCatalogByIndex(righttab, tabindex1)
                }
            }
        }
        function showMapWindow(objectid){
            var tabview = activeSplit ===1 ? lefttab : righttab
            mapWindow.setSource("visualization/MapWindow.qml",{"width" : tabview.width, "height" : tabview.height})

            var tab = tabview.getTab(tabview.currentIndex)
            if ( tab && tab.item.manager){
                mapWindow.item.transferLayers(tab.item.manager)
                mapWindow.item.show()
                closeTab(activeSplit,tabview.currentIndex)
            }
        }

        function showObject(objectid){
            var component = Qt.createComponent("visualization/Visualize.qml")
            var resource = mastercatalog.id2Resource(objectid)
            if ( resource !== null){
                var name = resource.displayName
                var blocksize = 24 / 2;
                if ( name.length > 15){
                    var part1 = name.substr(0,blocksize)
                    var part2 = name.substr( name.length - blocksize)
                    name = part1 + "..." + part2

                }
                var tabCount = righttab.count
                var tab = activeSplit ===1 ? righttab.addTab(name,component) : lefttab.addTab(name,component)
                tab.active = true
                if ( activeSplit ===1){
                    righttab.width = parent.width / 2.0;
                    righttab.state = "halfsize"
                    tabCount = righttab.count - 1 // tab has already been added so -1
                    righttab.currentIndex = tabCount
                    activeSplit = 2
                }
                else {
                    lefttab.width = parent.width / 2.0;
                    lefttab.state = "halfsize"
                    tabCount = lefttab.count - 1 // tab has already been added so -1
                    lefttab.currentIndex = tabCount
                    activeSplit = 1
                 }

                tab.item.addDataSource(resource.url, resource.name, resource.typeName)
                mastercatalog.setActiveTab(activeSplit, tabCount)
            }
        }

        function changeCatalog(url){
            var catalogpanel = getCurrentCatalogTab()
            if ( catalogpanel)
            {
                if ( catalogpanel.currentCatalog)
                    catalogpanel.currentCatalog.destroy(0)
                catalogpanel.currentCatalog = mastercatalog.newCatalog(url)
                catalogpanel.currentCatalog.makeParent(catalogpanel)
                var name = catalogpanel.currentCatalog.displayName
                var  tabview = activeSplit ===1 ? lefttab : righttab
                if ( tabview.currentIndex < tabview.count){
                    var tab = tabview.getTab(tabview.currentIndex)
                    if ( tab){
                        tab.title = name
                    }
                }
                mastercatalog.setWorkingCatalog(url);
                mastercatalog.currentCatalog = catalogpanel.currentCatalog
            }
        }

        function newCatalog(url, splitside) {

            if ( url){
                var component = Qt.createComponent("catalog/CatalogPanel.qml")
                var catalogModel = mastercatalog.newCatalog(url)
                var name = catalogModel.displayName
                var tabview = activeSplit ===1 ? lefttab : righttab
                var tab = activeSplit ===1 ? righttab.addTab(name,component) : lefttab.addTab(name,component)
                tab.active = true
                var tabCount = 0
                if ( activeSplit ===1){
                    if ( righttab.count == 1)
                        lefttab.state = "halfsize"
                    righttab.state = "halfsize"
                    activeSplit = 2
                    tab.item.tabLocation = "right"
                    tab.item.currentCatalog = catalogModel
                    tabCount = righttab.count - 1 // tab has already been added so -1
                    righttab.currentIndex = tabCount
                }
                else{
                    if ( splitside === -100) // start situation
                        righttab.state = "zerosize"
                    else if ( lefttab.count == 1)
                        righttab.state = "halfsize"
                    lefttab.state = righttab.count == 0 ? "fullsize" : "halfsize"
                    activeSplit = 1
                    tab.item.tabLocation = "left"
                    tab.item.currentCatalog = catalogModel
                    tabCount = lefttab.count - 1
                    lefttab.currentIndex = tabCount
                }
                tabview = activeSplit ===1 ? lefttab : righttab
                mastercatalog.setActiveTab(activeSplit, tabCount)
                mastercatalog.currentCatalog = catalogModel
            }
        }

        function addModeller(name) {
            var component = Qt.createComponent("modeller/ModellerPanel.qml")
            var tab = activeSplit ===1 ? righttab.addTab(name,component) : lefttab.addTab(name,component)
            tab.active = true
            if ( activeSplit ===1){
                righttab.width = parent.width / 2.0
                activeSplit = 2
            }
            else{
                lefttab.width = parent.width / 2.0
                activeSplit = 1
            }
        }

        function removeTabFromView(name) {
            righttab.removeTabFor(name);
            lefttab.removeTabFor(name);
        }


        DataTabView2 {
            id : lefttab
            side : 1
            Layout.fillWidth: true

            Component.onCompleted: {
                 newCatalog(mastercatalog.currentUrl,-100)
            }
        }


        DataTabView2{
            id : righttab
            side : 2
        }
    }

}
