import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MessageModel 1.0
import ResourceModel 1.0
import UIContextModel 1.0
import ".." as Base
import "catalog" as Catalog
import "modeller" as Modeller
import "../workbench" as Workbench



Rectangle {

    id: centerItem
    color: background4
    height : parent.height - 16
    width : bigthing.width - buttonB.width - infoP.width - 5
    property int activeSplit : 1

    property string prefix: "Modeller for "

    function addModellerPanel() {
        mainsplit.addModeller()
    }


    function removeModellerPanel() {
        mainsplit.removeModeller()
    }

    function iconSource(name) {
        if ( name.indexOf("/") !== -1)
            return name

         var iconP = "../images/" + name
         return iconP

     }

    function addCatalog() {
        mainsplit.addCatalog()
    }

    function addNewTab(name) {
        if (!modellerPaneExists(name)) {
            tabs.addTab(modellerTitle(name),Qt.createComponent("modeller/ModellerPanel.qml"))
            tabs.currentIndex++
            return true;
        }
        return false;
    }

    function showModellerPane(name) {
        if (modellerPaneExists(name)) {
            for (var i = 0; i < tabs.count; i++) {
                if (tabs.getTab(i).title === modellerTitle(name))
                    tabs.currentIndex = i
            }
        }
    }

    function removeTab(name) {
        if (modellerPaneExists(name)) {
            tabs.removeTab(getTabIndex(name))
        }
    }

    function getTabIndex(name) {
        for (var i = 0; i < tabs.count; i++) {
            if (tabs.getTab(i).title === modellerTitle(name))
                return i
        }
    }

    function modellerTitle(name) {
        return prefix + name
    }

    function modellerTitleRemovePrefix(name) {
        if (isModellerTitle(name))
            return name.substring(prefix.length, name.length)
    }

    function isModellerTitle(name) {
        return name.indexOf(prefix) === 0
    }

    function modellerPaneExists(name) {
        for (var i = 0; i < tabs.count; i++) {
            if (tabs.getTab(i).title === modellerTitle(name))
                return true
        }
        return false
    }

    ToolBar{
        id : tabtools
        width : parent.width
        height : 22
        Button {
            height : 20
            width : 20
            id : addSplit
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 2
            Image {
                anchors.fill: parent
                source : iconSource("plus.png")
                fillMode: Image.PreserveAspectFit
            }
//            onClicked: {
//                mainsplit.addModeller()
//            }
        }
        Button {
            height : 18
            width : 18
            id : removeSplit
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: addSplit.left
            anchors.rightMargin: 4
            Image {
                anchors.fill: parent
                source : iconSource("minus.png")
                fillMode: Image.PreserveAspectFit
            }
            onClicked: {
                console.log("minus")
            }
        }

    }

    SplitView {
        id : mainsplit
        orientation: Qt.Horizontal
        anchors.bottom: parent.bottom
        anchors.top : tabtools.bottom
        width : parent.width

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
                var tab = activeSplit ===1 ? righttab.addTab(name,component) : lefttab.addTab(name,component)
                tab.active = true
                if ( activeSplit ===1)
                    righttab.width = parent.width / 2.0
                else
                    lefttab.width = parent.width / 2.0

                tab.item.addSource(resource.url, resource.typeName)
            }
        }

        function addCatalog() {
            var component = Qt.createComponent("catalog/CatalogPanel.qml")
            var currentcatalog = mastercatalog.selectedCatalog()
            if ( currentcatalog !== null){
                mastercatalog.addCatalog(mastercatalog.currentUrl, mastercatalog.activeSplit === 0 ? 1 : 0)
                var name = currentcatalog.displayName
                var blocksize = 24 / 2;
                if ( name.length > 15){
                    var part1 = name.substr(0,blocksize)
                    var part2 = name.substr( name.length - blocksize)
                    name = part1 + "..." + part2
                }
                console.log(name)
                console.log()
                var tab = activeSplit ===1 ? righttab.addTab(name,component) : lefttab.addTab(name,component)
                tab.active = true
                console.log("NAME: ", name)
                console.log("TAB: ", tab)
                if ( activeSplit ===1){
                    righttab.width = parent.width / 2.0
                    activeSplit = 2
                    tab.item.tabLocation = "right"
                }
                else{
                    lefttab.width = parent.width / 2.0
                    activeSplit = 1
                    tab.item.tabLocation = "left"
                }
            }
        }
        function addModeller() {
            var component = Qt.createComponent("modeller/ModellerPanel.qml")
            var tab = activeSplit ===1 ? righttab.addTab("Modeller",component) : lefttab.addTab("Modeller",component)
            tab.active = true
            if ( activeSplit ===1){
                righttab.width = parent.width / 2.0
                activeSplit = 2
                //tab.item.tabLocation = "right"
            }
            else{
                lefttab.width = parent.width / 2.0
                activeSplit = 1
                //tab.item.tabLocation = "left"
            }
       }

        TabView {
            id : lefttab
            height : parent.height
            width: parent.width
            Layout.fillWidth: true

            onCurrentIndexChanged : {
              }

            style: Base.TabStyle2{
                splitindex: 1
                onSplitindexChanged: {
                    activeSplit = 1
                    mastercatalog.activeSplit = 0
                }
            }

            Tab {
                id : catalog_0
                title : "Catalog"
                Catalog.CatalogPanel{
                }
            }

        }
        TabView{
            id : righttab
            width : 0
            height : parent.height

            onCurrentIndexChanged : {
            }

            style: Base.TabStyle2{
                splitindex: 2
                onSplitindexChanged:{
                    activeSplit = 2
                    mastercatalog.activeSplit = 1
                }
            }
        }
    }

    focus : true
    Keys.onPressed: {
        console.debug(event.key)
        //uicontext.currentKey = event.key
    }
}
