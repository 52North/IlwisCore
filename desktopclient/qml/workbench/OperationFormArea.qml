import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import "../controls" as Controls
import "../Global.js" as Global

Rectangle {
    id : applicationForm
    height : 0
    x : parent.x + 5

    function newForm(metaid, title, url){
        operationid = metaid
        var form= formbuilder.index2Form(metaid, true)
        appFrame.formQML = form
        appFrame.formTitle = title
        appFrame.opacity = 1
    }

    ApplicationForm{
        id : appFrame
         width : parent.width
        height : parent.height - 30 < 0 ?  0 : parent.height - 30
        opacity : 0
    }
    Button{
        y : parent.height - 25
        width : 60
        text : "execute"
        height : 25
        x : parent.width - 60
        onClicked: appFrame.doExecute(operationid, false)
    }
    states: [
        State { name: "maximized"

            PropertyChanges {
                target: applicationForm
                height : appFrame ? appFrame.height : 350
                opacity : 1

            }
        },
        State {
            name : "minimized"
            PropertyChanges {
                target: applicationForm
                height : 0
                opacity : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
            NumberAnimation { properties: "opacity"; duration : 750 ; easing.type: Easing.InOutCubic }
        }
    ]
}

