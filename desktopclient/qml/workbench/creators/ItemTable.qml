import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

TableView {
    id : itemtable
    width : parent.width
    height : parent.height - 45
    selectionMode : SelectionMode.ExtendedSelection

    TableViewColumn{
        title : qsTr("Item name");
        role : "name"
        width : 150
        delegate: Component{
            Text {
                text: styleData.value
                verticalAlignment:Text.AlignVCenter
                font.pixelSize: 10
                elide: Text.ElideMiddle
                x : 4
            }
        }
    }
    TableViewColumn{
        title : qsTr("Code");
        role : "code"
        width : 30
        delegate: Component{
            Text {
                text: styleData.value
                verticalAlignment:Text.AlignVCenter
                font.pixelSize: 10
                elide: Text.ElideMiddle
            }
        }
    }
    TableViewColumn{
        title : qsTr("Description");
        role : "description"
        width : parent ? parent.width - 140 : 0
        delegate: Component{
            Text {
                text: styleData.value
                verticalAlignment:Text.AlignVCenter
                font.pixelSize: 10
                elide: Text.ElideMiddle
            }
        }
    }
    rowDelegate: Rectangle {
        id : rowdelegate
        height : 15
        color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")
    }

}

