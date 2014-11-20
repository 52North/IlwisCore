import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MessageModel 1.0
import ResourceModel 1.0
import QtQuick 2.0
import "catalog" as Catalog
import "modeller" as Modeller
import "../workbench" as Workbench



Rectangle {

    id: centerItem
    color: background4
    height : parent.height - 16
    width : bigthing.width - buttonB.width - infoP.width - 5

    property string prefix: "Modeller for "

    function iconSource(name) {
        if ( name.indexOf("/") !== -1)
            return name

         var iconP = "../images/" + name
         return iconP

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
        }

    }

    TabView {
        anchors.top : tabtools.bottom
        width : parent.width
        anchors.bottom: parent.bottom

        id : tabs
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
                    var tab = addTab(name,component)
                    currentIndex++
                    transitionInfoPane("Visualization.qml")
                }
            }

            function objectSelected(objectid){

            }

            onCurrentIndexChanged: {
                if (currentIndex > 0) {
                    var title = tabs.getTab(currentIndex).title
                    if (isModellerTitle(title))
                        updateSelectedItem(modellerTitleRemovePrefix(title))
                }
            }

           Tab {
            id : catalog_0
            title : "Catalog"
            Catalog.CatalogPanel{
            }
        }
    }
}
