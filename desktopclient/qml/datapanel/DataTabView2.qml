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

Item {
    id : datatabview
    property int side : 1
    property int currentIndex : 0
    property int count : datatab.count
    property bool stateDone : true

    onWidthChanged: {
        if ( !stateDone)    {
            datatabview.state = ""
            stateDone = true
        }
    }

    onSideChanged: {
        sideChange()
    }

    onCurrentIndexChanged: {
        datatab.currentIndex = currentIndex
    }

    Button{
        id : szbut
        width : datatab.count > 0 ? 0 : 0
        height : parent.height
        x : side == 1 ? 0 : parent.width - width
        Image {
            anchors.verticalCenter: szbut.verticalCenter
            width : 12
            height : 80
            source : side == 1 && width > 0 ? "../images/arrowleftlight.png" : "../images/arrowrightlight.png"
        }
    }
    DropArea {
        keys: [ "datatab" ]
        anchors.left: side == 1 ? szbut.right : parent.left
        anchors.leftMargin: 5
        height : 20
        width: parent.width - szbut.width - 5
        onDropped:{
            var createParameters = drop.source.sourcePanel.createParameters
            datapanesplit.newPanel(createParameters[0],createParameters[2],createParameters[1],side==1 ? "left" : "right")
            if ( drop.source.sourcePanel.panelType === "mappanel") {
                var tab = datatab.getTab(datatab.count - 1)
                var datapanel = tab.item
                datapanel.transfer(drop.source.sourcePanel)
            }
        }
        z: 10
    }

    TabView {
        id : datatab
        anchors.left: side == 1 ? szbut.right : parent.left
        height : parent.height
        width: parent.width - szbut.width - 5

        style: Base.TabStyle2 {
            id : tabstyle
            Connections {
                target: tabstyle
                onExpandToWindow: {
                    action : datapanesplit.showTabInFloatingWindow(panelside,tabindex)
                }
            }
            Connections {
                target: tabstyle
                onFullSize: {
                    action : datatabview.state = datatabview.state == "fullsize" ? "halfsize" : "fullsize"

                }
            }
        }
    }
    states: [
        State { name: "fullsize"

            PropertyChanges {
                target: datatabview
                width : parent.width
            }
        },
        State { name: "smallsize"

            PropertyChanges {
                target: datatabview
                width : 1 * parent.width / 4
            }
        },
        State { name: "halfsize"

            PropertyChanges {
                target: datatabview
                width : parent.width / 2
            }
        },
        State { name: "bigsize"

            PropertyChanges {
                target: datatabview
                width : 3 * parent.width / 4
            }
        },
        State {
            name : "zerosize"
            PropertyChanges {
                target: datatabview
                width : 0
            }
        }

    ]
    transitions: [
        Transition {
            id : widthTransition
            NumberAnimation { properties: "width"; duration : 500 ; easing.type: Easing.InOutCubic }
            onRunningChanged: {
                stateDone = widthTransition.running
            }
        }
    ]

    function sideChange() {
        centerItem.activeSplit = Math.abs(side)
        mastercatalog.activeSplit = Math.abs(side) - 1
        var tab = centerItem.getCurrentCatalogTab()
        if ( tab ){
            if ( "currentCatalog" in tab){
                mastercatalog.currentUrl = tab.currentCatalog.url
                mastercatalog.currentCatalog = tab.currentCatalog
            }
        }
    }

    function canSeparate(index){
        if ( index < datatab.count && index >= 0) {
            var tab = datatab.getTab(index)
            if ( tab && tab.item && tab.item.canSeparate)
                return true;
        }
        return false;
    }

    function addTab(name, component){
        return datatab.addTab(name, component)
    }

    function getTab(index){
        return datatab.getTab(index)
    }

    function insertTab(index, title, comp){
        return datatab.insertTab(index, title, comp)
    }

    /*
     * Remove tab by index
     */
    function removeTab(index){
        datatab.removeTab(index)
    }

    /*
     * Remove tab by name
     */
    function removeTabFor(name){
        for (var i = 0; i <  datatab.count; i++) {
            if (getTab(i).title === name) {
                 return removeTab(i);
            }
        }
    }

    /*
     * Get tab by name
     */
    function getTabIndexFor(name){
        for (var i = 0; i <  datatab.count; i++) {
            if (getTab(i).title === name) {
                 return i;
            }
        }
        return -1;
    }




}

