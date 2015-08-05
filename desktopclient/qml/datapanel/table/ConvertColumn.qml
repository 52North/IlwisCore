import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../workbench" as WorkBench

Rectangle {
    width: 300
    height: parent ? parent.height - 10 : 0
    property var operation

    Column {
        id : col
        y : 8
        width : 200
        height : 60
        spacing : 4
        ComboBox{
            id : targets
            width : 200
            height : 20
            model :  ["identifier","thematic","time","float","integer","color"]
        }
        Row {
            height : 20
            width : 200
            spacing : 40
            CheckBox{
                id : check
                width : 60
                height : 20
                checked : false
                text : qsTr("Domain name")
            }
            TextField{
                width : 100
                height : 20
                enabled : check.checked
                opacity : check.checked ? 1 : 0.25
            }
        }

    }
    Button{
        width : 80
        height : 22
        text : qsTr("Apply")
        anchors.right : col.right
        anchors.rightMargin: 5
        anchors.top : col.bottom
        onClicked: {
            var oldIndex =  columnlist.currentIndex
            var paramaters = {columnname:table.roleName(columnIndex+1),targetdomaintype : targets.currentText}
            operation.execute(paramaters)
            columnlist.setCurrentIndex(oldIndex)
        }
    }


}

