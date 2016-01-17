import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import UIContextModel 1.0
import "../../Global.js" as Global

Rectangle {
    id : buttonB
    height : 25
    color : Global.alternatecolor2
    property string side

    Row {
        anchors.fill: parent
        z : 1
        CatalogButton{
            id : showobj
            label: qsTr("View")
            onClicked: {
                var ids = mastercatalog.selectedIds()
                var idlist = ids.split("|")
                for(var i=0; i < idlist.length; ++i){
                    showObject(idlist[i]);
                }
            }
        }
        CatalogButton{
            id : oper
            //action: operClicked
            label: qsTr("Copy")
        }
        CatalogButton{
            id : modeller
            //action: modellerClicked
            label: qsTr("Move")
        }

        CatalogButton{
            id : deleteobj
            //action : propertiesClicked
            label: qsTr("Delete")
            onClicked: {
                var ids = mastercatalog.selectedIds()
                var idlist = ids.split("|")
                for(var i=0; i < idlist.length; ++i){
                   mastercatalog.deleteObject(idlist[i]);
                }
            }

        }
        CatalogButton{
            id : refresh
            label: qsTr("Refresh")
            onClicked: {
                mastercatalog.refreshCatalog(currentCatalog.url)
            }

        }
    }
}

