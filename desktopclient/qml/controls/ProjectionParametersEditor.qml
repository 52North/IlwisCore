import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import ResourceModel 1.0
import IlwisObjectModel 1.0

TableView {
    width : parent.width
    height : 100
    Component {
        id : readonlytext
        Text {
            anchors.fill: parent
            text: styleData.value
            verticalAlignment:Text.AlignVCenter
            font.pixelSize: 10
            elide: Text.ElideMiddle
        }
    }

    Component {
        id : readwritetext
        TextField {
            anchors.fill: parent
            text: styleData.value
            verticalAlignment:Text.AlignVCenter
            font.pixelSize: 10
        }
    }

    TableViewColumn{
        title : qsTr("Projection Parameter");
        role : "name"
        width : 120
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
        title : qsTr("Value");
        role : "value"
        width : 150
        property bool editState : false
        delegate: editState ? readwritetext : readonlytext
    }
    rowDelegate: Rectangle {
        id : rowdelegate
        height : 20
        color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")
    }
}

