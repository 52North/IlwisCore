import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Rectangle {
    property var itemdomain
    anchors.fill: parent

    onItemdomainChanged: {
        if ( itemdomain) {
            itemtable.model = itemdomain.domainitems
        }
    }

    Column{
        width : parent.width
        height : parent.height + 20
        spacing: 3
        Text {
            text : qsTr("Available items")
            font.bold: true
            height : 20
        }
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
        Button{
            text : qsTr("Add items")
            anchors.right : parent.right
            anchors.rightMargin: 4
            width : 70
            height : 18
            onClicked: {
                var items = []
                itemtable.selection.forEach( function(rowIndex) {
                    var item = itemtable.model[rowIndex].name
                    items.push(item)
                })
                addDomainItems(items)
            }
        }
    }
}

