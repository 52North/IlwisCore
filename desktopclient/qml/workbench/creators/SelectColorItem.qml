import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Rectangle {
    property var itemdomain
    anchors.fill: parent

    onItemdomainChanged: {
        if ( itemdomain) {
            itemtable.model = itemdomain.domainitems
        }
    }

    Column{
        width : parent.width
        height : parent.height + 20
        spacing: 3
        Text {
            text : qsTr("Available items")
            font.bold: true
            height : 20
        }

        PaletteColorList{
            id : itemtable
        }

        Button{
            text : qsTr("Add items")
            anchors.right : parent.right
            anchors.rightMargin: 4
            width : 70
            height : 18
            onClicked: {
                var items = []
                itemtable.selection.forEach( function(rowIndex) {
                    var item = itemtable.model[rowIndex].name
                    items.push({name: item})
                })
                addDomainItems(items, true)
            }
        }
    }
}

