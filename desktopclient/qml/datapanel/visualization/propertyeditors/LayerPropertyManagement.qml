import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global

Rectangle {
    id : propertyEditorBar

    property var renderer
    property int layerIndex : 0

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name

        if ( name === "")
            name = "redbuttonr.png"

         var iconP = "../../images/" + name
         return iconP
     }

    Layout.minimumHeight: 16

    Rectangle {
        id : header
        width : parent.width
        height : 18
        color : Global.headerdark
        Text {
            text : "Display Options"
            font.bold: true
            font.pointSize: 8
            anchors.left : parent.left
            anchors.leftMargin: 5
            color : "white"
        }
    }

    Item {
        anchors.top : header.bottom
        width : parent.width - 5
        height : parent.height - 18
        x : 5

        VisualAttributeEditorLayers{
            id : layerColumn
            height : parent.height
            width : 180
            anchors.left: parent.left


        }

        VisualAttributeEditorAttributeList{
            id : attributeListColumn
            anchors.left : layerColumn.right
            anchors.leftMargin: 3
            height : parent.height - 16
        }

        VisualAttributeEditorList{
            id : editorListColumn
            anchors.left : attributeListColumn.right
            anchors.leftMargin: 3
            height : parent.height - 16
        }
        VisualAttributeEditor{
            anchors.left : editorListColumn.right
            anchors.leftMargin: 3
            anchors.right: parent.right
            anchors.rightMargin: 5
            height : parent.height - 16
            id : editorColumn
        }
    }

}





