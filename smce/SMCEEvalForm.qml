import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.1
import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1

import LayersView 1.0
import SMCE 1.0
import "../../../qml/Global.js" as Global
import "../../../qml/controls" as Controls

Column {
    id: evalform
    anchors.fill: parent
    anchors.margins: 5

    Text {
        text: selectedNode ? selectedNode.name : ""
        width: parent.width
        wrapMode: Text.Wrap
    }

    Item {
        width: parent.width
        height: 5
    }

    Column {
        id: weights
        spacing: 5
        visible: selectedNode ? (selectedNode.type === Node.Group) : false

        Text {
            text : qsTr("Weights")
            font.bold : true
        }

        ListView {
            spacing: 5
            anchors.margins: 5
            model: (selectedNode && selectedNode.weights !== null) ? selectedNode.weights.directWeights : null
            width: parent.width
            height: childrenRect.height
            delegate: Row {
                spacing: 5
                anchors.margins: 5
                TextField {
                    id : directWeight
                    width : 30
                    text : ""
                    font.pointSize: 8

                    property string oldText

                    style: TextFieldStyle {
                        background: Rectangle {
                            radius: 3
                            width : parent.width
                            height: parent.height
                            border.color: parent.enabled ? Global.edgecolor : "transparent"
                            border.width: directWeight.readOnly ? 0: 1
                            color : "white"
                        }
                    }
                    onTextChanged: {
                        var regex = /^-?\d*(\.\d*)?$/
                        if (!regex.test(text)){
                            text = oldText
                        } else {
                            oldText = text
                            model.directWeight = text
                        }
                    }
                    Component.onCompleted: {
                        text = model.directWeight
                    }
                }

                Text {
                    id : normalizedWeight
                    width: 30
                    text : model.normalizedWeight.toFixed(2).toString()
                    font.pointSize: 8
                }

                Text {
                    id : label
                    width : evalform.width - directWeight.width - normalizedWeight.width
                    text : model.name
                    font.weight: Font.Bold
                    font.pointSize: 8
                    elide: Text.ElideRight
                    clip :true
                }
            }
        }

        Row {
            Button {
                text : qsTr("Apply")
                onClicked: {
                    if (selectedNode && selectedNode.weights)
                        selectedNode.weights.apply()
                }
            }

            Button {
                text : qsTr("Cancel")
                onClicked: {
                    if (selectedNode)
                        selectedNode.modelData.resetWeightEdits()
                }
            }
        }
    }

    Column {
        id: standardization
        visible: selectedNode ? (selectedNode.type !== Node.Group) : false
        width: parent.width
        spacing: 5

        Text{
            text : standardization.guiText(selectedNode)
            font.bold : true
        }

        Loader {
            id : stdEditor
            width: parent.width
            source: standardization.qmlFile(selectedNode)
        }

        Row {
            Button {
                text : qsTr("Apply")
                onClicked: {
                    if (selectedNode)
                        selectedNode.standardization.apply()
                }
            }

            Button {
                text : qsTr("Cancel")
                onClicked: {
                    if (selectedNode) {
                        selectedNode.modelData.resetStandardizationEdits()
                        if (stdEditor && stdEditor.item)
                            stdEditor.item.refresh()
                    }
                }
            }
        }

        function qmlFile(selectedNode) {
            if (selectedNode) {
                if (selectedNode.standardization) {
                    switch(selectedNode.standardization.type) {
                    case Standardization.Value:
                        return "SMCEGraphStandardization.qml"
                    case Standardization.ValueConstraint:
                        return "SMCEValueConstraintStandardization.qml"
                    case Standardization.Class:
                        return "SMCEClassStandardization.qml"
                    case Standardization.ClassConstraint:
                        return "SMCEClassConstraintStandardization.qml"
                    case Standardization.Bool:
                        return "SMCEBoolStandardization.qml"
                    case Standardization.BoolConstraint:
                        return "SMCEBoolConstraintStandardization.qml"
                    default:
                        return ""
                    }
                } else
                    return ""
            } else
                return ""
        }

        function guiText(selectedNode) {
            if (selectedNode) {
                if (selectedNode.standardization) {
                    switch(selectedNode.standardization.type) {
                    case Standardization.Value:
                        return qsTr("Standardization of Value Indicator")
                    case Standardization.ValueConstraint:
                        return qsTr("Standardization of Value Constraint")
                    case Standardization.Class:
                        return qsTr("Standardization of Class Indicator")
                    case Standardization.ClassConstraint:
                        return qsTr("Standardization of Class Constraint")
                    case Standardization.Bool:
                        return qsTr("Standardization of Boolean Indicator")
                    case Standardization.BoolConstraint:
                        return qsTr("Standardization of Boolean Constraint")
                    default:
                        return qsTr("Standardization: unknown")
                    }
                } else
                    return qsTr("Standardization: no input map selected")
            } else
                return qsTr("Standardization")
        }
    }

    function selNodeAboutToChange() {
        if (selectedNode) {
            if (selectedNode.type !== Node.Group)
                selectedNode.modelData.resetStandardizationEdits()
            else
                selectedNode.modelData.resetWeightEdits()
        }
    }

    function selNodeChanged() {
        if (selectedNode && selectedNode.type !== Node.Group)
            if (stdEditor && stdEditor.item)
                stdEditor.item.refresh()
    }
}
