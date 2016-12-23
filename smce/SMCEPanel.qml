import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import CatalogModel 1.0
import SMCE 1.0

import "../../../qml/controls" as Controls

Column {
    id : column
    anchors.fill: parent
    property alias toolbar : tools
    property CatalogModel smceCatalog

    function loadSmceCatalog() {
        scanCatalog(uicontext.ilwisFolder + "/smcedata")
    }

    function scanCatalog(folder) {
        smceCatalog = mastercatalog.newCatalog(folder, "container='" + folder + "'")
    }

    function openMap(mapname) {
        var resources = smceCatalog.resources
        var sz = resources.length
        var newTab = null
        if (mapname != "") {
            for(var i = 0; i < sz; ++i) {
                var resource = resources[i]
                if (resource.name == mapname && resource.typeName == "rastercoverage") {
                    newTab = datapanesplit.newPanel("itemid=" + resource.id, resource.typeName, resource.url, "left") // "left" to open to the left, "other", to open in the other tab, regardless of our tab
                }
            }
        }
        return newTab
    }

    SMCEToolBar{
        id : tools        
    }

    //signal nodeChanged (string node)

    GroupBox {
        id: mode
        Layout.fillWidth: true
        RowLayout {
            ExclusiveGroup { id: modeGroup }
            RadioButton {
                id: defModeButton
                text: qsTr("Evaluation definition mode")
                checked: true
                exclusiveGroup: modeGroup
                Layout.minimumWidth: 100
                onClicked: {
                    message("defMode")
                }
            }
            RadioButton {
                id: evalModeButton
                text: qsTr("Evaluation mode")
                exclusiveGroup: modeGroup
                Layout.minimumWidth: 100
                onClicked: {
                    message("evalMode")
                }
            }
            /*RadioButton {
                id: analysisModeButton
                text: qsTr("Analysis mode")
                exclusiveGroup: modeGroup
                Layout.minimumWidth: 100
            }*/
        }
    }

    Rectangle {
        id : smcearea
        width : parent.width
        height : parent.height - tools.height
        color : "#FFF8DC"
        opacity : height > 0 ? 1 : 0

        Tree {
            id: evalTree
            width : parent.width
            height : parent.height

            onSelNodeChanged: {
                console.log("onSelNodeChanged signal received...")
                nodeChanged(node)
            }
        }
    }
}
