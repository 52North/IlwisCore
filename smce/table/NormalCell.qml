import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1

import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls

Rectangle {
    function cellColor(columnIndex, rowIndex, selected){
        if ( selected)
            return Global.selectedColor
        var rowBand = (rowIndex % 10) > 4
        var colBand = (columnIndex) % 2 == 1
        if ( rowBand && colBand)
            return "#E4EBE5"
        if ( rowBand)
            return "#EEF2EE"
        if ( colBand)
            return "#F8FAF8"
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
        visible: !editable
    }

    TextField {
        id : txtInp
        width : parent.width - 5
        height : parent.height
        text: styleData.value ? styleData.value : ""
        horizontalAlignment: table.isNumericalColumn(styleData.column) ? Text.AlignRight : Text.AlignLeft
        x : 2
        visible: editable
        validator: RegExpValidator { regExp: !isBoolStandardization ? /^-?\d*(\.\d*)?$/ : /^(?i)(true|false)$/ }

    }

    Component.onCompleted: {
    }
}



