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

TabView {
    id : datatab
    height : parent.height
    width: parent.width
    property int side : 1
    property int indexTab : 0
    property string dataType : ""

    onSideChanged: {
        centerItem.activeSplit = side
        mastercatalog.activeSplit = Math.abs(side) - 1
        datatab.currentIndex = indexTab
        var tab = centerItem.getCurrentCatalogTab()
        if ( tab && tab.item && tab.item.currentCatalog){
            mastercatalog.currentUrl = tab.currentCatalog.url
            mastercatalog.currentCatalog = tab.currentCatalog
        }
    }

    onIndexTabChanged: {
        currentIndex = indexTab
    }

    style: Base.TabStyle2{
    }

    states: [
        State { name: "fullsize"

            PropertyChanges {
                target: datatab
                width : parent.width
            }
        },
        State { name: "halfsize"

            PropertyChanges {
                target: datatab
                width : parent.width / 2
            }
        },
        State {
            name : "zerosize"
            PropertyChanges {
                target: datatab
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

