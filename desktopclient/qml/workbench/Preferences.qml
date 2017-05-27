import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global
import "./preferences" as Preferences
import "../controls" as Controls

WorkBenchShifter {
    id : messageContainer

    clip : true
    state : "invisible"
    height : parent.height

    FunctionBarHeader{
        id : functionBar
        headerImage: "../images/settings.png"
        headerText:"Settings"
    }
    Column {
        anchors.top : functionBar.bottom
        anchors.topMargin: 4
        height : parent.height - functionBar.height - 4
        width : parent.width

        Column{
            width : parent.width
            height : childrenRect.height
            Preferences.PreferencesButton{
                connectedItem: locs
                imageSource : "../images/folderdisk.png"
                labelText : qsTr("Locations")
            }

            Preferences.Locations{
                id : locs
            }
        }


        Column{
            width : parent.width
            height : childrenRect.height

            Preferences.PreferencesButton{
                imageSource : "../images/visualizationCS1.png"
                labelText : qsTr("User Interface")
                connectedItem: displayOptions


            }
            Preferences.DisplayOptions{
                id : displayOptions
            }
        }
        Preferences.PreferencesButton{
            imageSource : "../images/mapview.png"
            labelText : qsTr("Map view")
        }
        Preferences.PreferencesButton{
            connectedItem: locs
            imageSource : "../images/table20.png"
            labelText : qsTr("Table view")
        }

        Preferences.PreferencesButton{
            imageSource : "../images/users20.png"
            labelText : qsTr("Users")
        }
        Preferences.PreferencesButton{
            imageSource : "../images/font20.png"
            labelText : qsTr("Fonts")
        }
    }
}

