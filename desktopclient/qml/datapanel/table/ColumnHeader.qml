import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import TableModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Rectangle {
    color: styleData.selected ? "#4668A6" : "#C1D5F8"
    border.color:  Global.headerdark
    implicitWidth: Math.max(text.width + 4, 100)
    implicitHeight: Global.rowHeight
    Text {
        id: text
        anchors.centerIn: parent
        text: styleData.value
        font.weight: Font.DemiBold
        color: styleData.selected ? "white" : "black"

//        MouseArea{
//            anchors.fill: parent
//            onEntered: {
//                console.debug("aaaaa")
//                table.selectColumn(styleData.column)
//            }
//        }
    }

}

