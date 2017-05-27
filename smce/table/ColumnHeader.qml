import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import TableModel 1.0
import "../../../../qml/controls" as Controls
import "../../../../qml/Global.js" as Global

Rectangle {
    id : header
    color: uicontext.darkestColor
    objectName: uicontext.uniqueName()
    border.color:  Global.headerdark
    width: defaultWidth(styleData.column)
    height: Global.rowHeight
    property bool selected : false
    Text {
        id: text
        property int count : 0
        text: styleData.value
        font.weight: Font.DemiBold
        color: styleData.pressed ? uicontext.lightColor : "white"
        y : 3
        x : 4

        onColorChanged: {
            count += 1
            if ( count  % 2 == 0){
                selected = !selected
                table.selectColumn(styleData.column, selected)
                header.color = table.isColumnSelected(styleData.column) ? "#4668F6" : "#C1D5F8"
                table.update()
             }

            console.log("ColumnHeader :" + count)
        }
    }

    Component.onCompleted: {
    console.log("ColumnHeader : onCompleted]")
    }
}

