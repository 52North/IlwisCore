import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base

Item {
    id : layersinfo
    width : viewmanager.width
    height: viewmanager.height

    function iconSource(name) {
        if ( name === "")
            return ""
         var iconP = "../../images/" + name
         return iconP

     }

    GridView{
        anchors.fill: parent
        model : manager.layerInfoItems
        cellWidth : 240
        cellHeight : 15
        flow: GridView.FlowTopToBottom
        clip : true
        delegate: Component{
            Item{
                width : 260
                height : 15
                Row {
                    spacing: 3
                    Image {
                        source : iconSource(icon)
                        width : 14
                        height : 14
                    }

                    Text{
                        text : name
                        width : 100
                        font.bold : true
                    }
                    Text {
                        text : value
                        width : 140
                        clip : true

                    }
                }
            }
        }

    }
}

