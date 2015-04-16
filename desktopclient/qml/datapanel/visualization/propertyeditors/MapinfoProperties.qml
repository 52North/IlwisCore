import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../.." as Base

Item {
    width: 100
    height: 62
    property var editor

    CheckBox{
        id : usesInfo
        x : 5
        y : 5
        text: "Show mouse over map info"

        style: Base.CheckBoxStyle1{}

        checked : editor.showInfo

        onCheckedChanged: {
            editor.showInfo = checked
        }
    }

}

