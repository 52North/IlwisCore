import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayersView 1.0
import SMCE 1.0
import "../../../qml/Global.js" as Global
import "../../../qml/controls" as Controls

Column {
    id: editColumn
    anchors.fill: parent
    anchors.margins: 5
    spacing: 5

    Text {
        text : (selectedNode != null) ? ((selectedNode.type === Node.Group) ? ((selectedNode.level === 0) ? qsTr("Goal") : qsTr("Group")) : ((selectedNode.type === Node.Constraint) ? qsTr("Constraint") : ((selectedNode.type === Node.Factor) ? qsTr("Factor") : ((selectedNode.type === Node.MaskArea) ? qsTr("Mask") : "")))) : ""
        font.bold : true
        anchors.margins: 5
    }

    TextArea {
        id: nameField
        text : selectedNode != null ? selectedNode.name : ""
        width: parent.width
        height: parent.height / 3
    }

    Controls.TextEditLabelPair{
        id : unitField
        visible: selectedNode != null && selectedNode.type !== Node.Group
        labelWidth: 40
        labelText: qsTr("Unit")
        content: selectedNode ? selectedNode.unit : ""
        transparentBackgrond: false
    }

    Button {
        text : qsTr("Apply")
        onClicked: {
            selectedNode.name = nameField.text
            selectedNode.unit = unitField.content
        }
    }

    Button {
        text : qsTr("Add Group")
        visible: selectedNode != null && selectedNode.type === Node.Group
        onClicked: {
            selectedNode.modelData.addGroup("New Group ...")
            selectedItem.selectLastChild()
        }
    }
    Button {
        text : qsTr("Add Factor")
        visible: selectedNode != null && selectedNode.type === Node.Group
        onClicked: {
            selectedNode.modelData.addFactor("New Factor ...")
            selectedItem.selectLastChild()
        }
    }
    Button {
        text : qsTr("Add Constraint")
        visible: selectedNode != null && selectedNode.type === Node.Group
        onClicked: {
            selectedNode.modelData.addConstraint("New Constraint ...")
            selectedItem.selectLastChild()
        }
    }
    Button {
        text : qsTr("Add Mask Area")
        visible: selectedNode != null && selectedNode.type === Node.Group && selectedNode.level === 0
        onClicked: {
            selectedNode.modelData.addMask("New Mask Area ...")
            selectedItem.selectLastChild()
        }
    }
    Button {
        text : qsTr("Delete")
        onClicked: {
            selectedNode.modelData.deleteNode()
        }
    }

    function selNodeAboutToChange() {

    }

    function selNodeChanged() {

    }
}
