import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import TableModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Rectangle {
    width : childrenRect.width
    height : childrenRect.height
    color : "lightgrey"
    Text {
        text: styleData.value
        verticalAlignment:Text.AlignVCenter
        color : styleData.selected ? "#2121A3" : "black"
        horizontalAlignment: Text.AlignLeft
    }
}

