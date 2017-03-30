import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import TableModel 1.0
import TabModel 1.0
import "../../../qml/controls" as Controls
import "../../../qml/Global.js" as Global


Item {
    id : tablePane
    width: parent.width
    height : parent.height
    property bool canSeparate : true
    property string panelType : "tablepanel"
    property string stdAttrName : "cityblock" // "std"
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

    function addDataSource(sourceUrl, filter, sourceType){
        console.log("addDataSource 2 called....")
        console.log("tablePane 2: " + tablePane)

        console.log("sourceUrl 2: " + sourceUrl)
        console.log("sourceType 2: " + sourceType)

        table = uicontext.createTableModel(tablePane,sourceUrl, sourceType)
        console.log("table 2: " + table)
        if ( table){
            console.log("table 2: table created")
            tableView.model = table
            console.log("2 table.columnCount: " + table.columnCount)
            for(var i =0; i < table.columnCount; ++i){
                if ( i == 0)
                    tableView.addColumn(column.createObject(tableView,{"role" : "first", "title" : "nr", "width" : defaultWidth(i)}))
                else {
                    var roleName = table.roleName(i)
                    var dw = table.defaultWidth(i)
                    tableView.addColumn(column.createObject(tableView,{"role" : roleName, "title" : roleName, "width" : dw, "editable" : roleName === stdAttrName }))


                }
            }
            createParameters = [sourceUrl, filter, sourceType]
            return table.id
        } else {
            console.log("table 2: table not created")
        }
    }


    TableView {
        id : tableView
        width : parent.width
        height : parent.height - 40
        //selectionMode : SelectionMode.ExtendedSelection

        headerDelegate : ColumnHeader{}


        rowDelegate: Rectangle {
            id : rowdelegate
            height : 20
            color : styleData.selected ? Global.selectedColor :  (((styleData.row % 10) > 4)? "#eee" : "#fff")
        }

        /*states: [
            State { name: "visible"

                PropertyChanges {
                    target: tableView
                    height : parent.height //- 270
                }
            },
            State {
                name : "invisible"
                PropertyChanges {
                    target: tableView
                    height : parent.height  //- 24
                }
            }

        ]
        transitions: [
            Transition {
                NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
            }
        ]*/

        Component.onCompleted : {
            console.log("tableView parent.height is: "+ parent.height)
        }
    }

    Button {
        id: stdGo
        text: "Go"
        anchors.top: tableView.bottom
        anchors.right: tableView.right

        onClicked: {
            //executeStandardization()
        }
    }

    Component.onCompleted: {
        console.log("tableView.columnCount: " + tableView.columnCount)
        console.log("tableView.rowCount: " + tableView.rowCount)
    }
}
