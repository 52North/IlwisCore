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


    Item {
        width : parent.width - 5
        height : parent.height
        y : 2

        LayersList{
            id : layerColumn
            height : parent.height
            width : parent.width / 4
            anchors.left: parent.left


        }

        VisualAttributeList{
            id : attributeListColumn
            anchors.left : layerColumn.right
            anchors.leftMargin: 3
            height : parent.height - 3
            width : 140
        }

        VisualAttributeEditorList{
            id : editorListColumn
            anchors.left : attributeListColumn.right
            anchors.leftMargin: 3
            height : parent.height - 3
            width : 140
        }
        VisualAttributeEditor{
            anchors.left : editorListColumn.right
            anchors.leftMargin: 3
            anchors.right: parent.right
            anchors.rightMargin: 5
            height : parent.height - 3
            id : editorColumn
        }
    }

}





