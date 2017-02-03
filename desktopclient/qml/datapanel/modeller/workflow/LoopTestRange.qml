import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Row {
    height : 25
    property var data2
    property int index
    property var container
    property var choices

    ComboBox {
        x: 2
        height : 20
        width : parent.width -115
        model: choices
    }


    Component.onCompleted: {
        var id = operations.operationId("ilwis://operations/range");
        var operation = operations.operation(id)
        choices  = operation.choiceList(0);
    }
}
