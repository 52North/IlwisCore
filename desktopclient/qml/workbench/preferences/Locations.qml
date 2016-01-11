import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import PreferencesModel 1.0

Column {
    id : locationsPane
    width: parent.width
    height: 0
    opacity : 0
    state : "minimized"

    onStateChanged: {
        if ( state == "minimized"){
            cacheFolders.state = "minimized"
        }
    }

    Connections {
        target: cacheFolders
        onResultFolder :{
            preferences.cacheLocation = folder
        }
    }

    Connections {
        target: internalFolders
        onResultFolder :{
            preferences.internalCatalog = folder
        }
    }

    Controls.FolderSelection {
        id : cacheFolders
        x : 4
        width : parent.width - 24
        labelText: qsTr("Cache")
        labelWidth: 100


        Component.onCompleted: {
            currentFolder = preferences.cacheLocation
            startFolder = currentFolder
        }
    }

    Controls.FolderSelection {
        id : internalFolders
        x : 4
        width : parent.width - 24
        labelText: qsTr("Internal Catalog")
        labelWidth: 100


        Component.onCompleted: {
            currentFolder = preferences.internalCatalog
            startFolder = currentFolder
        }
    }

    Controls.FolderSelection {
        id : sharedFolder
        x : 4
        width : parent.width - 24
        labelText: qsTr("Shared Folder")
        labelWidth: 100


    }


    states: [
        State { name: "maximized"

            PropertyChanges {
                target: locationsPane
                height : 250
                opacity : 1
            }
        },
        State {
            name : "minimized"
            PropertyChanges {
                target: locationsPane
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

