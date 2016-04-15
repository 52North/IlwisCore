import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Rectangle {
    width: parent.width
    height: 35

    Row {
        width : parent.width
        height : 31
        Controls.ActionButtonH {
            width : parent.width / 4
            height : parent.height
            buttontext : qsTr("Run")
            iconsource : "../images/run20.png"

            onClicked: {
                workflow.executeForm(false)
            }
        }
        Controls.ActionButtonH {
            id : stepButton
            property bool init : true
            width : parent.width / 4
            height : parent.height
            buttontext : qsTr("Step")
             iconsource : "../images/step20.png"
             onClicked :{
                if ( stepButton.init){
                    workflow.executeForm(true)
                    stepButton.init = false
                }
                else
                    modellerDataPane.stepMode()
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

