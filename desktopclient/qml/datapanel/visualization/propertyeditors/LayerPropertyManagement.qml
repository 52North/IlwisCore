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


    SplitView {
        width : parent.width - 5
        height : parent.height
        y : 2

        LayersList{
            id : layerColumn
            height : parent.height
            width : 140
            anchors.left: parent.left


        }

        VisualAttributeList{
            id : attributeListColumn
            height : parent.height - 3
            width : 120
        }

        VisualAttributeEditorList{
            id : editorListColumn
            height : parent.height - 3
            width : 120
        }
        VisualAttributeEditor{
            height : parent.height - 3
            Layout.fillWidth: true
            id : editorColumn
        }
    }

}





