import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import TableModel 1.0
import "../../../../qml/controls" as Controls
import "../../../../qml/Global.js" as Global

TableViewColumn{
    width : table.defaultWidth(styleData.column)
    property bool editable: false
    property bool isBoolStandardization : false

    delegate : Component {
        Loader{
            source: styleData.row < table.recordCount  ?  (styleData.column >=1 ? "NormalCell.qml" : "FirstColumn.qml"): "AggregateCell.qml"
        }
    }
}

