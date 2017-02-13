import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayersView 1.0
import SMCE 1.0
import "../../../qml/Global.js" as Global
import "../../../qml/controls" as Controls

Item {
    Column {
        id: editColumn
        anchors.fill: parent
        anchors.margins: 5
        spacing: 5

        Text {
            text : (selectedNode.type === Node.Group) ? ((selectedNode.level === 0) ? qsTr("Goal") : qsTr("Group")) : ((selectedNode.type === Node.Constraint) ? qsTr("Constraint") : ((selectedNode.type === Node.Factor) ? qsTr("Factor") : ((selectedNode.type === Node.MaskArea) ? qsTr("Mask") : "")))
            font.bold : true
            anchors.margins: 5
        }

        TextArea {
            id: nameField
            text : selectedNode.name
            width: parent.width
        }

        Controls.TextEditLabelPair{
            id : unitField
            visible: selectedNode.type !== Node.Group
            //editWidth: 80
            labelWidth: 40
            labelText: qsTr("Unit")
            content: selectedNode.unit
            transparentBackgrond: false
        }

        Button {
            id : applyGoalBut
            height : 22
            text : qsTr("Apply")

            onClicked: {
                selectedNode.name = nameField.text
                selectedNode.unit = unitField.content
            }
        }
    }
}
