import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Column {
    height : 160
    width : parent ? parent.width : 0
    spacing : 4
    Button{
        anchors.right: parent.right
        anchors.rightMargin: 2
        width : 80
        height : 22
        text : "New Item"
        onClicked: {
            newItem.enabled = true
        }
    }
    Text {
        width : parent.width
        height : 20
        text : "Palette Item"
        opacity : newItem.enabled ? 1 : 0
    }


    Item {
        id :newItem
        enabled : false
        opacity : enabled ? 1.0 : 0
        width : parent.width
        height : 20
        Text {
            id : colorlabel
            width : 100
            height : parent.height
            text : qsTr("Color")
        }

        Controls.ColorPicker2{
            id : palettecolorPicker
            anchors.left : colorlabel.right

        }

        Button{
            anchors.right: parent.right
            anchors.rightMargin: 2
            anchors.top : palettecolorPicker.bottom
            anchors.topMargin: 4
            width : 80
            height : 22
            text : "Add Item"
            onClicked: {
                newItem.enabled = false
                var items = []
                var v = palettecolorPicker.selectedColor.toString() // else javascript pass all by reference
                items.push({paletteColor: v})
                addDomainItems(items, false)
            }

        }
    }


}




