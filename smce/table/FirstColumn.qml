import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import TableModel 1.0
import "../../../../qml/controls" as Controls
import "../../../../qml/Global.js" as Global

Rectangle {
    width : defaultWidth(styleData.column)
    height : 20
    color : styleData.selected ? Global.selectedColor :  (((styleData.row % 10) > 4)? Global.darkgreen: Global.middlegreen )
    Text {
        text: styleData.value
        anchors.verticalCenter: parent.verticalCenter
        color : styleData.selected ? "#2121A3" : "white"
        horizontalAlignment: Text.AlignLeft
        x : 4
        font.bold: true
    }
}

