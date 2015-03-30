import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import TableModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Item {
    id : tablePane
    width: parent.width
    height : parent.height

    property bool canSeparate : true
    property TableModel table

    Component{
        id : column
        TableColumn{
        }
    }

    function addDataSource(sourceUrl, sourceName, sourceType){
        table = uicontext.createTableModel(tablePane,sourceUrl, sourceType)
        tableView.model = table
        tableView.addColumn(column.createObject(tableView,{"role" : "first", "title" : "nr", "width" : 25}))
        for(var i =0; i < table.columnCount; ++i){
            var roleName = table.roleName(i)
            var defaultWidth = table.defaultWidth(i)
            tableView.addColumn(column.createObject(tableView,{"role" : roleName, "title" : roleName, "width" : defaultWidth}))
        }
    }

    TableView {
        id : tableView
        anchors.fill : parent
        selectionMode : SelectionMode.ExtendedSelection

        headerDelegate : ColumnHeader{
        }

        
        rowDelegate: Rectangle {
            id : rowdelegate
            height : 20
            color : styleData.selected ? Global.selectedColor :  (((styleData.row % 10) > 4)? "#eee" : "#fff")
        }

    }
}

