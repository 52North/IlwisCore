import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0

Rectangle {
    id : catalogProperties
    width : parent.width
    anchors.margins: 6
    color : "white"
    border.width: 1
    border.color: "#E6E6E6"
    property string humannametext
    property string defintiontext
    property string descriptiontext

    signal bookmarksChanged()

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name

         var iconP = "../../images/" + name
         return iconP
     }

    Action {
        id :maxButtons
        onTriggered: {
            catalogProperties.state = "maximized"
        }
    }

    Action {
        id :minButtons
        onTriggered: {
            catalogProperties.state = "minimized"
        }
    }

    Action {
        id :saveProperties
        onTriggered: {
            mastercatalog.setCatalogMetadata(displayString.text, descriptionString.text)
            bookmarksChanged()
        }
    }

    Image {
        id : title
        width : parent.width
        height : 24
        source : iconsource("headerblue2CS1.png")
    Text {
        text : "Properties"
        font.bold : true
        x : 5
        y : 5
        width : 60
    }
    }
    Row {
        height : title.height
        anchors.right : catalogProperties.right
        width : 75
        Button{
            id : save
            height : 20
            width :20
            action : saveProperties
            Image { anchors.centerIn : parent; width : 16; height : 16; source: iconsource("minisave.png"); smooth : true}

        }
        Button{
            id : full
            height : 20
            width :20
            action : maxButtons
            Image { anchors.centerIn : parent; source: iconsource("max1.png") }
        }
        Button{
            id : close
            height : 20
            width :20
            action : minButtons
            Image { anchors.centerIn : parent; source: iconsource("min1.png") }
        }
    }

    ColumnLayout {
        anchors.top : title.bottom
        anchors.bottom : parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5
        TextField  {
            id : displayString
            placeholderText: "Human readable alias"
            Layout.minimumWidth: parent.width
            text : humannametext
            style: TextFieldStyle {
                textColor: "black"
                background: Rectangle {
                    radius: 4
                    implicitWidth: 100
                    implicitHeight: 24
                }
            }

        }
        Text {
            id : urlTitle
            text : "Definition"
        }

        TextArea {
            id : definitionString
            Layout.minimumWidth: parent.width
            Layout.maximumHeight: 40
            text : defintiontext
        }
        Text {
            id : descTitle
            text : "Description"
        }

        TextArea {
            id : descriptionString
            Layout.minimumWidth: parent.width
            anchors.top : descTitle.bottom
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            text : descriptiontext
        }
    }

    Component.onCompleted: catalogProperties.state = "minimized"

    states: [
        State { name: "maximized"

            PropertyChanges {
                target: catalogProperties
                height : 200
            }
        },
        State {
            name : "minimized"
            PropertyChanges {
                target: catalogProperties
                height : 25
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
        }
    ]

}
