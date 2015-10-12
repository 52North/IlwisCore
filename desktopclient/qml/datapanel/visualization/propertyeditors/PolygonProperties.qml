import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../.." as Base
import "../../../controls" as Controls

Item {
    width : Math.min(300,parent ? parent.width : 300)
    height : Global.rowHeight * 2 + 20
    property var editor
    y : 10
    x : 10

    opacity: width > 120 ? 1 : 0

    Column {

        spacing : 3
        width : parent.width
        height : parent.height - 10

        CheckBox{
            id : showAreas
            text: qsTr("Show Areas")
            checked : editor.showAreas

            style: Base.CheckBoxStyle1{}

            onCheckedChanged: {
                editor.showAreas = showAreas.checked
            }
        }
    }
}

