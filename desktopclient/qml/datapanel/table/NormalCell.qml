import QtQuick 2.2
import "../../Global.js" as Global

Rectangle {
    width : defaultWidth(styleData.column)
    height : 18
    color : ( styleData.selected || table.isColumnSelected(styleData.column)) ? Global.selectedColor : ((( styleData.row % 10) > 4)? "#eee" : "#fff")

    Text {
        text: styleData.value ? styleData.value : ""
        color : ( styleData.selected || table.isColumnSelected(styleData.column)) ? "#2121A3" : "black"
        elide: Text.ElideMiddle
        x : 2
    }
}



