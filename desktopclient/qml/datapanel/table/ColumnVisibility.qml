import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

Column {
    id : cv
    property var operation
    property int columnIndex

    property var columnmodel : table.columns

    width : 200
    height : parent.height
    TableView {
        id : columnlist
        width : 200
        height :   parent.height - 40
        selectionMode : SelectionMode.ExtendedSelection
        model : columnmodel

        property var vis : []

        onModelChanged: {
            // a bit a hack as the update in tableview isnt ok in qt 5.2 (known issue)
             if ( columnlist.vis.length == 0){
                     for(var i=0; i < table.columnCount; ++i){
                     columnlist.vis.push(true)
                 }
             }
        }

        anchors.topMargin: 4
        x : 3
        clip : true

        TableViewColumn{
            id : nameColumn
            role : "attributename"
            title : qsTr("Column Name")
            width : 120
            delegate: Component{
                Row {
                    width : 120
                    height  :20
                    CheckBox {
                        enabled : false
                        width : 20
                        height : 20
                        checked : columnlist.vis[styleData.row] //tableView.getColumn(styleData.row).visible
                    }

                    Text {
                        text: styleData.value
                        verticalAlignment:Text.AlignVCenter
                        font.pixelSize: 11
                        color : styleData.selected ? "white" : "black"
                        elide: Text.ElideMiddle
                    }
                }
            }
        }
    }
    Button {
        text : "apply"
        width : 120
        height : 25

        onClicked: {
            if (columnlist.selection){
               columnlist.selection.forEach(function(rowIndex) { columnlist.vis[rowIndex] = !columnlist.vis[rowIndex]})
               columnlist.model = null
               columnlist.model = columnmodel
                for(var j=0; j < table.columnCount; ++j){
                   tableView.getColumn(j).visible = columnlist.vis[j]
                }
            }
        }
    }
}
