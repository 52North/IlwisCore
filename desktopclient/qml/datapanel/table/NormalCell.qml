import QtQuick 2.2

Text {
    width : defaultWidth(styleData.column)
    text: styleData.value
    color : ( styleData.selected || table.isColumnSelected(styleData.column)) ? "#2121A3" : "black"
    elide: Text.ElideMiddle
    x : 2
}



