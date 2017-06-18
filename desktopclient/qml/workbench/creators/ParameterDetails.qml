import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base
import UIContextModel 1.0

Column {
    spacing: 3
    Controls.TextEditLabelPair{
        id : namefield
        width : parent.width
        labelWidth: 100
        labelText: qsTr("Name")
    }
    Controls.ComboxLabelPair{
        width : parent.width
        id : datatypefield
        labelText: qsTr("Data type")
        labelWidth: 100
        itemModel: uicontext.typeNames
    }

    Controls.TextEditLabelPair{
        id : descField
        width : parent.width
        labelWidth: 100
        labelText:  qsTr("Description")
    }
    Row {
        width : parent.width
        height : 22
        spacing : 4
        Button{
            id : addButton
            width : 100
            height : 20
            text : qsTr("Add")
            onClicked: {
                if (parameterlistField.inputActive)
                    inputparameterList.append({"name" : namefield.content,"valuetype" : datatypefield.comboText,"description" : descField.content })
                else
                    outputparameterList.append({"name" : namefield.content,"valuetype" : datatypefield.comboText,"description" : descField.content })

                details.visible = false
            }
        }
        Button{
            width : 100
            height : 20
            text : qsTr("Close")
            onClicked : {
               details.visible = false
            }
        }
    }
}
