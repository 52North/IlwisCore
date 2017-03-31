import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../qml/controls" as Controls
import "../../../qml/Global.js" as Global


Column {
    id : createForm
    width : stdEditor.width
    spacing: 1

    property double minX: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValueConstraint !== null) ? selectedNode.standardization.standardizationValueConstraint.min : 0
    property double maxX: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValueConstraint !== null) ? selectedNode.standardization.standardizationValueConstraint.max : 0
    property double minVal: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValueConstraint !== null) ? selectedNode.standardization.standardizationValueConstraint.minVal : 0
    property double maxVal: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValueConstraint !== null) ? selectedNode.standardization.standardizationValueConstraint.maxVal : 0
    property bool useMin: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValueConstraint !== null) ? selectedNode.standardization.standardizationValueConstraint.useMin : false
    property bool useMax: (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValueConstraint !== null) ? selectedNode.standardization.standardizationValueConstraint.useMax : false

    function refresh() {
        minimum.checked = useMin
        maximum.checked = useMax
        minText.text = minVal.toFixed(3).toString()
        maxText.text = maxVal.toFixed(3).toString()
    }

    Text {
        text: qsTr("The minimum is ") + minX.toString()
    }

    Text {
        text: qsTr("The maximum is ") + maxX.toString()
    }

    Text {
        text: qsTr("Pass on the following condition(s):")
    }

    Row {
        CheckBox {
            id: minimum
            text: qsTr("Minimum")
            onClicked: {
                if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValueConstraint !== null)
                    selectedNode.standardization.standardizationValueConstraint.useMin = checked
            }
            Component.onCompleted: {
                checked = useMin
            }
        }
        TextField {
            id: minText
            text : ""
            font.pointSize: 8
            visible: useMin

            property string oldText

            style: TextFieldStyle {
                background: Rectangle {
                    radius: 3
                    width : parent.width
                    height: parent.height
                    border.color: Global.edgecolor
                    border.width: 1
                    color : "white"
                }
            }
            onTextChanged: {
                var regex = /^-?\d*(\.\d*)?$/
                if (!regex.test(text))
                    text = oldText
                else {
                    if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValueConstraint !== null)
                        selectedNode.standardization.standardizationValueConstraint.minVal = parseFloat(text)
                    oldText = text
                }
            }
            onEditingFinished: {
                text = minVal.toFixed(3).toString()
            }
            Component.onCompleted: {
                text = minVal.toFixed(3).toString()
            }
        }
    }

    Row {
        CheckBox {
            id: maximum
            text: qsTr("Maximum")
            onClicked: {
                if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValueConstraint !== null)
                    selectedNode.standardization.standardizationValueConstraint.useMax = checked
            }
            Component.onCompleted: {
                checked = useMax
            }
        }
        TextField {
            id: maxText
            text : ""
            font.pointSize: 8
            visible: useMax

            property string oldText

            style: TextFieldStyle {
                background: Rectangle {
                    radius: 3
                    width : parent.width
                    height: parent.height
                    border.color: Global.edgecolor
                    border.width: 1
                    color : "white"
                }
            }
            onTextChanged: {
                var regex = /^-?\d*(\.\d*)?$/
                if (!regex.test(text))
                    text = oldText
                else {
                    if (selectedNode && selectedNode.standardization !== null && selectedNode.standardization.standardizationValueConstraint !== null)
                        selectedNode.standardization.standardizationValueConstraint.maxVal = parseFloat(text)
                    oldText = text
                }
            }
            onEditingFinished: {
                text = maxVal.toFixed(3).toString()
            }
            Component.onCompleted: {
                text = maxVal.toFixed(3).toString()
            }
        }
    }

    Text {
        text: qsTr("Resulting condition: ") + getCondition()
    }

    function getCondition() {
        if (!(useMin || useMax))
            return "<>0";
        else if (useMin && !useMax)
            return qsTr("Min") + "=" + minVal.toString();
        else if (!useMin && useMax)
            return qsTr("Max") + "=" + maxVal.toString();
        else { // (useMin && useMax)
            if (minVal <= maxVal)
                return qsTr("Inside") + "[" + minVal.toString() + "," + maxVal.toString() + "]"
            else // maxVal < minVal
                return qsTr("Outside") + "(" + maxVal.toString() + " ," + minVal.toString() + ")"
        }
    }
}
