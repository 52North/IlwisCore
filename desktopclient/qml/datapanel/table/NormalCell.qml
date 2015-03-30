import QtQuick 2.2

Text {
    text: styleData.value
    verticalAlignment:Text.AlignVCenter
    color : ( styleData.selected || table.isColumnSelected(styleData.column)) ? "#2121A3" : "black"
    elide: Text.ElideMiddle
}



