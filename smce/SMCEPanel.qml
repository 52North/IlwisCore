import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import SMCE 1.0

import "../../../qml/controls" as Controls

Column {
    id : column
    anchors.fill: parent
    property alias toolbar : tools

    SMCEToolBar{
        id : tools        
    }

    signal modeChanged(string newMode)

    GroupBox {
        id: mode
        Layout.fillWidth: true
        RowLayout {
            ExclusiveGroup { id: modeGroup }
            RadioButton {
                id: defModeButton
                text: qsTr("Evaluation definition mode")
                checked: true
                exclusiveGroup: modeGroup
                Layout.minimumWidth: 100
                onCheckedChanged: {
                    modeChanged("defMode")
                }
            }
            RadioButton {
                id: evalModeButton
                text: qsTr("Evaluation mode")
                exclusiveGroup: modeGroup
                Layout.minimumWidth: 100
                onCheckedChanged: {
                    modeChanged("evalMode")
                }
            }
            /*RadioButton {
                id: analysisModeButton
                text: qsTr("Analysis mode")
                exclusiveGroup: modeGroup
                Layout.minimumWidth: 100
            }*/
        }
    }

    Rectangle {
        id : smcearea
        width : parent.width
        height : parent.height - tools.height
        color : "#FFF8DC"
        opacity : height > 0 ? 1 : 0

        Tree {
            id: evalTree
            width : parent.width
            height : parent.height
        }
    }
}
