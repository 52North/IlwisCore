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
            id : workspace
            //action: workspaceClicked
            label: qsTr("View")
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
            id : prop
            //action : propertiesClicked
            label: qsTr("Delete")

        }
        CatalogButton{
            id : newcat
            //action : propertiesClicked
            label: qsTr("Refresh")

        }
    }
}

