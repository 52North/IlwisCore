import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

GridView {
    id : itemtable
    width : parent.width
    height : 100
    cellHeight : 16
    cellWidth : 55
    flow : GridView.FlowTopToBottom

    delegate: Component{
        Row {
            width : 55
            height : 16
            Text {
                x : 2
                text : index + " :"
                width : 20
            }

            Rectangle {
                y : 2
                width : 25
                height : 12
                color : paletteColor
                border.width: 1
            }
        }
    }



}

