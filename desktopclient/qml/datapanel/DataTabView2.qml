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

Item{
    id : datatabview
    property int side : 1
    property int currentIndex : 0
    property int count : datatab.count


    onSideChanged: {
        centerItem.activeSplit = Math.abs(side)
        mastercatalog.activeSplit = Math.abs(side) - 1
        var tab = centerItem.getCurrentCatalogTab()
        if ( tab && tab.item && tab.item.currentCatalog){
            mastercatalog.currentUrl = tab.currentCatalog.url
            mastercatalog.currentCatalog = tab.currentCatalog
        }
    }

    function canSeparate(index){
        if ( index < datatab.count && index >= 0) {
            var tab = datatab.getTab(index)
            if ( tab && tab.item.canSeparate)
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

    function removeTab(index){
        datatab.removeTab(index)
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

    TabView {
        id : datatab
        anchors.left: side == 1 ? szbut.right : parent.left
        height : parent.height
        width: parent.width - szbut.width

        style: Base.TabStyle2{
        }
    }
    states: [
        State { name: "fullsize"

            PropertyChanges {
                target: datatabview
                width : parent.width
            }
        },
        State { name: "halfsize"

            PropertyChanges {
                target: datatabview
                width : parent.width / 2
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
            NumberAnimation { properties: "width"; duration : 500 ; easing.type: Easing.InOutCubic }
        }
    ]


}

