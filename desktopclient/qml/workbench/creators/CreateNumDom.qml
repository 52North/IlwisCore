import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Rectangle {
    height: 100
    border.width : 1
    border.color : Global.edgecolor
    radius: 5

    Column {
        width : parent.width - 7
        height : 70
        y : 5
        spacing : 2
        x : 3
        Controls.TextEditLabelPair{
            labelText: qsTr("Name")
            labelWidth: 100
            width : parent.width
        }

        Controls.TextEditLabelPair{
            labelText: qsTr("Minimum value")
            labelWidth: 100
            width : parent.width
        }

        Controls.TextEditLabelPair{
            labelText: qsTr("Maximum value")
            labelWidth: 100
            width : parent.width
        }

        Controls.TextEditLabelPair{
            labelText: qsTr("Resolution")
            labelWidth: 100
            width : parent.width
        }
    }
}

