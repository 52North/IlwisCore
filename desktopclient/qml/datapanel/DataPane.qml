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

    function addCatalog() {
        mainsplit.addCatalog()
    }

//    ToolBar{
//        id : tabtools
//        width : parent.width
//        height : 22
//        Button {
//            height : 20
//            width : 20
//            id : addSplit
//            anchors.verticalCenter: parent.verticalCenter
//            anchors.right: parent.right
//            anchors.rightMargin: 2
//            Image {
//                anchors.fill: parent
//                source : iconSource("plus.png")
//                fillMode: Image.PreserveAspectFit
//            }
//        }
//        Button {
//            height : 18
//            width : 18
//            id : removeSplit
//            anchors.verticalCenter: parent.verticalCenter
//            anchors.right: addSplit.left
//            anchors.rightMargin: 4
//            Image {
//                anchors.fill: parent
//                source : iconSource("minus.png")
//                fillMode: Image.PreserveAspectFit
//            }
//        }

//    }
    FocusScope {
        anchors.fill: parent

        SplitView {
            id : mainsplit
            orientation: Qt.Horizontal
            anchors.fill: parent


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
                    var tab = activeSplit ===1 ? righttab.addTab(name,component) : lefttab.addTab(name,component)
                    tab.active = true
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
                        lefttab.currentIndex = indexTab
                    }
                }

                Tab {
                    id : catalog_0
                    title : "Catalog"
                    Catalog.CatalogPanel{
                    }
                }

                function removeTabFor(name) {
                    var remove = -1;
                    var l = count;
                    for (var i = l-1; i >= 0; i--) {
                        if (getTab(i).title === name) {
                            remove = i;
                        }
                    }
                    if (remove >= 0) {
                        removeTab(remove);
                    }
                    if (count === 0) {
                        activeSplit = 2;
                        width = 0;
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
                        righttab.currentIndex = indexTab
                    }
                }

                function removeTabFor(name) {
                    var remove = -1;
                    var l = count;
                    for (var i = l-1; i >= 0; i--) {
                        if (getTab(i).title === name) {
                            remove = i;
                        }
                    }
                    if (remove >= 0) {
                        removeTab(remove);
                    }
                    if (count === 0) {
                        activeSplit = 1;
                        width = 0;
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
}
