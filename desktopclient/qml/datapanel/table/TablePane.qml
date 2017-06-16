import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import TableModel 1.0
import TabModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Item {
    id : tablePane
    width: parent.width
    height : parent.height
    property bool canSeparate : true
    property string panelType : "tablepanel"
    property var createParameters : []
    property TabModel tabmodel

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

    function transfer(datapanel){
        if ( datapanel.table)    {
            addDataSource(datapanel.table.url, "", "table")
        }
    }

    function addDataSource(sourceUrl, filter, sourceType){
        table = uicontext.createTableModel(tablePane,sourceUrl, sourceType)

        if ( table){
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
            createParameters = [sourceUrl, filter, sourceType]
            return table.id
        }
    }
    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical
        height : parent.height
        TableView {
            id : tableView
            width : parent.width
            height : parent.height - 270
            selectionMode : SelectionMode.ExtendedSelection

            headerDelegate : ColumnHeader{}


            rowDelegate: Rectangle {
                id : rowdelegate
                height : 20
                color : styleData.selected ? Global.selectedColor :  (((styleData.row % 10) > 4)? "#eee" : "#fff")
            }

            states: [
                State { name: "visible"

                    PropertyChanges {
                        target: tableView
                        height : parent.height - 270
                    }
                },
                State {
                    name : "invisible"
                    PropertyChanges {
                        target: tableView
                        height : parent.height  - 24
                    }
                }

            ]
            transitions: [
                Transition {
                    NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
                }
            ]

        }
        ColumnManagement{
            id : columnManagement
            height : 270
        }
    }
}

