import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0


Rectangle {
    id : buttonB
    y : 0
    height : parent.height
    width : 55
    color : "#c2caca"
    border.width : 1
    Layout.maximumWidth : 120

    //signal transitionInfoPane(string pagename)

    Action {
        id : navClicked
        onTriggered : {
            transitionInfoPane("Navigator.qml")
        }

    }

    Action {
        id : operClicked
        onTriggered : {
            transitionInfoPane("OperationSelection.qml")
        }

    }
    Action {
        id : propertiesClicked
        onTriggered : {
            transitionInfoPane("ObjectProperties.qml")
        }
    }
    Action {
        id : messagesClicked
        onTriggered : {
            transitionInfoPane("MessagesPane.qml")
        }
    }
    Action {
        id : progressClicked
        onTriggered : {
            transitionInfoPane("Tranquilizers.qml")
        }

    }
    Action {
        id : modellerClicked
        onTriggered : {
            transitionInfoPane("Modeller.qml")
        }

    }
    Action {
        id :maxButtons
        onTriggered: {
            buttonB.width = 55
        }
    }

    Action {
        id :minButtons
        onTriggered: {
            buttonB.width = 0
        }
    }

    Column {
        anchors.fill: parent
        Rectangle {
            height : 21
            width : buttonB.width
            color : background4
            Row {
                anchors.fill: parent
                Button{
                    id : full
                    height : 20
                    width :20
                    action : maxButtons
                    Image { anchors.centerIn : parent; source: "../images/max1.png" }
                }
                Button{
                    id : close
                    height : 20
                    width :20
                    action : minButtons
                    Image { anchors.centerIn : parent; source: "../images/min1.png" }
                }
            }
        }

        WorkBenchButton{
            id : nav
            action: navClicked
            iconname: "navigatorCS1.png"
        }

        WorkBenchButton{
            id : oper
            action: operClicked
            iconname : "operationCS1.png"
        }
        WorkBenchButton{
            id : modeller
            action: modellerClicked
            iconname : "modellerCS1.png"
        }
        WorkBenchButton{
            id : prop
            action : propertiesClicked
            iconname : "propertiesCS1.png"

        }
        WorkBenchButton{
            id : errors
            action : messagesClicked
            iconname : messagehandler.messageIcon
        }
        WorkBenchButton{
            id : progress
            action : progressClicked
            iconname : "progressCS1.png"

        }
        Button{
            id : create
            height : buttonB.width
            width : buttonB.width
            Image { anchors.centerIn: parent; source: "../images/createCS3.png" }

        }
        Button{
            id : config
            height : buttonB.width
            width : buttonB.width
            Image { anchors.centerIn: parent; source: "../images/configCS3.png" }

        }

    }
}
