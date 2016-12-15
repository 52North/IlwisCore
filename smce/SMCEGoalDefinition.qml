import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../qml/controls" as Controls
import "../../../qml/Global.js" as Global


Item {
    id : goalForm
    width : parent.width
    height : parent.height

    GroupBox {
        id: scale
        Layout.fillWidth: true
        ColumnLayout {
            ExclusiveGroup { id: scaleGroup }
            RadioButton {
                id: problemButton
                text: qsTr("Problem (1 -> 0)")
                checked: true
                exclusiveGroup: scaleGroup
                Layout.minimumWidth: 100
            }
            RadioButton {
                id: opportunityButton
                text: qsTr("Opportunity (0 -> 1)")
                exclusiveGroup: scaleGroup
                Layout.minimumWidth: 100
            }
        }
    }

    Controls.TextEditLabelPair{
        id : unitsfield
        labelWidth: 80
        labelText: qsTr("Units")
        transparentBackgrond: false
        anchors.top: scale.bottom
    }

    // Analysis area




    Button {
        id : applyBut
        height : 22
        text : qsTr("Apply")
        anchors.top: unitsfield.bottom
        onClicked: {

        }
    }

}

