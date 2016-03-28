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
            width : parent.width / 3
            height : parent.height
            buttontext : "Run"
            iconsource : "../images/run20.png"

            onClicked: {
                workflow.executeForm()
            }
        }
        Controls.ActionButtonH {
            width : parent.width / 3
            height : parent.height
            buttontext : "Step"
             iconsource : "../images/step20.png"
        }
        Controls.ActionButtonH {
            width : parent.width / 3
            height : parent.height
            buttontext : "Stop"
            iconsource : "../images/stop20.png"
        }

    }
}

