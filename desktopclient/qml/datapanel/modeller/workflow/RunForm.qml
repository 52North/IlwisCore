import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Column {
    width: parent.width
    height: 55
    spacing : 2
    property alias stepMode : stepModeCheck.checked
    Rectangle {
        height : 2
        width : parent.width - 20
        anchors.horizontalCenter: parent.horizontalCenter
        border.width: 1
        border.color: Global.edgecolor

    }

    CheckBox {
        id : stepModeCheck
        text : qsTr("Step Mode")
        checked: false
    }

    Row {
        width : parent.width
        height : 31
        Controls.ActionButtonH {
            width : parent.width / 4
            height : parent.height
            buttontext : qsTr("Run")
            iconsource : "../images/run20.png"

            onClicked: {
                var runid = modellerDataPane.workflowModel().runid()
                workflow.executeForm({"runid" : runid, "stepmode" : stepModeCheck.checked})
            }
        }
        Controls.ActionButtonH {
            id : stepButton
            width : parent.width / 4
            height : parent.height
            buttontext : qsTr("Step")
            enabled : stepModeCheck.checked
            opacity : enabled ? 1 : 0.5
             iconsource : "../images/step20.png"
             onClicked :{
                 if ( stepModeCheck.checked )
                    modellerDataPane.workflowModel().nextStep()
                    modellerDataPane.nextStep()
             }
        }
        Controls.ActionButtonH {
            width : parent.width / 4
            height : parent.height
            buttontext : qsTr("Stop")
            iconsource : "../images/stop20.png"
        }
        Controls.ActionButtonH {
            width : parent.width / 4
            height : parent.height
            buttontext : qsTr("Save")
            iconsource : "../images/save20.png"

            onClicked:{
                modellerDataPane.store()
            }
        }

    }
}

