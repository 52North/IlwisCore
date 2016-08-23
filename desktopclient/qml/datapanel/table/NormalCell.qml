import QtQuick 2.2
import "../../Global.js" as Global

Rectangle {
    function cellColor(columnIndex, rowIndex, selected){
        if ( selected)
            return Global.selectedColor
        var rowBand = (rowIndex % 10) > 4
        var colBand = (columnIndex) % 2 == 1
        if ( rowBand && colBand)
            return "#EDEBD6"
        if ( rowBand)
            return "#F4F3E6"
        if ( colBand)
            return "#f7f6ed"
        return "#fff"
    }

    width : defaultWidth(styleData.column)
    height : 18
    color : cellColor( styleData.column,styleData.row, (styleData.selected || table.isColumnSelected(styleData.column)))

    Text {
        id : txt
        width : parent.width - 5
        height : parent.height
        text: styleData.value ? styleData.value : ""
        color : ( styleData.selected || table.isColumnSelected(styleData.column)) ? "#2121A3" : "black"
        elide: Text.ElideMiddle
        horizontalAlignment: table.isNumericalColumn(styleData.column) ? Text.AlignRight : Text.AlignLeft
        x : 2
    }
}



