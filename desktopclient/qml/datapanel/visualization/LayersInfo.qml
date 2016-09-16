import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base

Column {
    id : layersinfo
    width : viewmanager.width
    height: viewmanager.height
    property string iconName : "../images/info_s"

    function iconSource(name) {
        if ( name === "")
            return ""
         var iconP = "../../images/" + name
         return iconP

     }
    Rectangle {
        id : header
        width : parent.width
        height : 18
        color : Global.palegreen
        Text{
            text : qsTr("Attribute information on Mouse Position")
            font.bold: true
            x : 3
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    GridView{
        width : parent.width
        height : parent.height - header.height - 2
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
                        color: layerHeader ? "darkblue" : "black"
                        elide: Text.ElideMiddle
                    }
                    Text {
                        text : value
                        width : 140
                        clip : true
                        color: layerHeader ? "darkblue" : "black"
                        font.bold : layerHeader ? true : false
                        elide: Text.ElideMiddle

                    }
                }
            }
        }

    }
}

