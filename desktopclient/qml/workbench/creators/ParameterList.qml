import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

TabView {
    width : parent.width
    height : parent.height - 45
    property bool inputActive : currentIndex == 0
    Tab {
        title : qsTr("Input parameters")

        TableView {
            id : inputTable
            width : parent.width
            height : parent.height
            model : inputparameterList

            TableViewColumn{
                id : inamecolumn
                title : qsTr("Name");
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
                id : itypeColumn
                title : qsTr("Valuetype");
                role : "valuetype"
                width : 80
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
                title : qsTr("Description");
                role : "description"
                width : parent.width - itypeColumn.width - inamecolumn.width
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
            rowDelegate: Rectangle {
                id : irowdelegate
                height : 15
                color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")
            }
        }

    }
    Tab {
        title : qsTr("Output parameters")

        TableView {
            id : outputTable
            width : parent.width
            height : parent.height
            model : outputparameterList

            TableViewColumn{
                id : onamecolumn
                title : qsTr("Name");
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
                id : otypeColumn
                title : qsTr("Valuetype");
                role : "valuetype"
                width : 80
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
                title : qsTr("Description");
                role : "description"
                width : parent.width - otypeColumn.width - onamecolumn.width
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
            rowDelegate: Rectangle {
                id : orowdelegate
                height : 15
                color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")
            }
        }

    }
    Component.onCompleted: {
        currentIndex = 0
    }
}
