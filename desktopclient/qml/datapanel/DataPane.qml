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
    property int activeTab : 1

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
        if ( tabview && activeTab > 0 && tabview.count > 0) {
            var tab = tabview.getTab(activeTab - 1)
            if ( tab && tab.item){
                if ( "currentCatalog" in tab.item)
                    return tab.item
            }
        }
    }


    function newCatalog(url, splitside){
        if ( url === ""){
            var catTab = getCurrentCatalogTab()
            url = catTab.currentCatalog.url
            splitside = activeSplit
        }

        mainsplit.newCatalog(url,splitside)
    }

    function setCatalogByIndex(currentTab, tabindex){
        currentTab.currentIndex = tabindex - 1
        activeTab = tabindex
        var catalogtab = getCurrentCatalogTab()
        if ( catalogtab.currentCatalog){
            mastercatalog.currentUrl = catalogtab.currentCatalog.url
            mastercatalog.currentCatalog = catalogtab.currentCatalog
        }
    }

    function changeCatalog(url){
       mainsplit.changeCatalog(url)
    }

    FocusScope {
        anchors.fill: parent

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
                        setCatalogByIndex(righttab, 1)

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
                        setCatalogByIndex(lefttab, 1)
                    }else{
                        setCatalogByIndex(righttab, tabindex1)
                    }
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
                        righttab.width = parent.width / 2.0
                        tabCount = righttab.count - 1 // tab has already been added so -1
                    }
                    else {
                        lefttab.width = parent.width / 2.0
                        tabCount = lefttab.count - 1 // tab has already been added so -1
                    }

                    tab.item.addSource(resource.url, resource.typeName)
                    mastercatalog.setActiveTab(activeSplit, tabCount)
                }
            }

            function changeCatalog(url){
                var tab = getCurrentCatalogTab()
                if ( tab){
                    if ( tab.currentCatalog)
                        tab.currentCatalog.destroy(0)
                    tab.currentCatalog = mastercatalog.newCatalog(url)
                    tab.currentCatalog.makeParent(tab)
                    mastercatalog.setWorkingCatalog(url);
                    mastercatalog.currentCatalog = tab.currentCatalog
                }
            }

            function newCatalog(url, splitside) {
                if ( url){
                    var component = Qt.createComponent("catalog/CatalogPanel.qml")
                    var catalogModel = mastercatalog.newCatalog(url)
                    var name = catalogModel.displayName
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
                        if ( splitside === -1) // start situation
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


            DataTabView {
                id : lefttab
                side : 1
                Layout.fillWidth: true

                Component.onCompleted: {
                    newCatalog(mastercatalog.currentUrl,-1)
                }
            }


            DataTabView{
                id : righttab
                side : 2
            }
        }
    }
}
