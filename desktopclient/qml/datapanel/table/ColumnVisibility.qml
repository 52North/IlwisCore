import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

Column {
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

        anchors.topMargin: 4
        x : 3
        clip : true

        TableViewColumn{
            id : nameColumn
            role : "attributename"
            title : qsTr("Column Name")
            width : 120
            delegate: Component{
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
    Button {
        text : "apply"
        width : 120
        height : 25

        onClicked: {
            if (columnlist.selection){
                columnlist.selection.forEach(function(rowIndex) {var col = tableView.getColumn(rowIndex);col.visible = !col.visible})
            }
        }
    }
}
