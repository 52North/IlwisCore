import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import "../../Global.js" as Global

Rectangle {
    id : catalogProperties
    width : parent.width
    anchors.margins: 6
    color : currentFolders.currentIndex === index ? "#99CCFF" : index  % 2 == 0 ? uicontext.lightestColor: "white"
    border.width: 1
    border.color: "#E6E6E6"
    property alias humannametext : displayString.text
    property string defintiontext
    property alias descriptiontext : descriptionString.text

    signal bookmarksChanged()

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name

        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../../images/" + name
        return iconP
    }


    Image {
        id : title
        width : parent.width
        height : 0
        source : iconsource("headerblue2CS1.png")
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
                height : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "height"; duration : 500 ; easing.type: Easing.InOutCubic }
        }
    ]

}
