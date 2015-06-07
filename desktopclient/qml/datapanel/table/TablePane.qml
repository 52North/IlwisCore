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

    property TableModel table

    Component{
        id : column
        TableColumn{
        }
    }

    function defaultWidth(index){
        if (!table)
            return 60
        return table.defaultWidth(index)
    }

    function addDataSource(sourceUrl, sourceName, sourceType){
        table = uicontext.createTableModel(tablePane,sourceUrl, sourceType)
        tableView.model = table
        columnManagement.setColumnModel1(table.columns)

        for(var i =0; i < table.columnCount; ++i){
            if ( i == 0)
                tableView.addColumn(column.createObject(tableView,{"role" : "first", "title" : "nr", "width" : defaultWidth(i)}))
            else {
                var roleName = table.roleName(i)
                var dw = table.defaultWidth(i)
                tableView.addColumn(column.createObject(tableView,{"role" : roleName, "title" : roleName, "width" : dw}))
            }
        }
    }
    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical
        TableView {
            id : tableView
            width : parent.width
            Layout.fillHeight: true
            selectionMode : SelectionMode.ExtendedSelection

            headerDelegate : ColumnHeader{
            }


            rowDelegate: Rectangle {
                id : rowdelegate
                height : 20
                color : styleData.selected ? Global.selectedColor :  (((styleData.row % 10) > 4)? "#eee" : "#fff")
            }

        }
        ColumnManagement{
            id : columnManagement
        }
    }
}

