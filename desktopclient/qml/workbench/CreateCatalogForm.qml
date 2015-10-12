import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import Qt.labs.folderlistmodel 2.1
import MasterCatalogModel 1.0
import ".." as Base
import "../controls" as Controls

Item {
    height : 0
    width : 0
    opacity: 0

    property string beginfolder
    property string currentFolder

    signal bookmarkadded()

    function path2pathView(pathString){
        var index =  pathString.lastIndexOf("/..")
        if ( index !== -1){
            pathString = pathString.slice(0,index) // chop the /..
            index = pathString.lastIndexOf("/")
            if ( index !== -1){
                pathString = pathString.slice(0,index)  // chop the last 'real'folder as that is what we want here
            }
        }

        var parts = pathString.split("/");
        pathModel.clear()
        for (var i = 0; i < parts.length; i++){
            pathModel.append({"folderid" : parts[i] + "/"})
        }

    }

    function parseFilePath(index){
        var scheme = "file:///";
        var path = currentFolder.slice(scheme.length())
        var parts = path.split("/");
        var newPath;
        for(var i=0; i < index; ++i){
            if ( i > 0)
                newPath += "/"
            newPath += parts[i]
        }
    }



    Component {
        id : pathViewDelegate
        Text  {
            height : 20
            text :folderid;
            verticalAlignment: Text.AlignVCenter
        }
    }

    ListModel {
        id: pathModel
    }

    id: catalogTypes
    TabView {
        id : tabs
        y : 5
        height : parent.height - addBookmarkButton.height - 15
        anchors.left: parent.left
        anchors.right : parent.right
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        clip : true
        Tab {
            id : filetab
            height: parent.height
            width : parent.width
            title : "File"
            Item {
                anchors.fill: parent
                Connections{
                    target: fileselection
                    onFolderChanged: { currentFolder = newfolder}
                }
                FileSelectionPanel{
                    id : fileselection
                    startFolder:  beginfolder
                }
            }
        }
        Tab {

            title : "Service"
            height: parent.height
            width : parent.width
            Item {
                anchors.fill: parent
                anchors.topMargin: 5
                SelectServiceForm{}
            }
        }
        Tab {
            title : "Database"
        }

        Tab {
            title : "Selection"
        }
        style: Base.TabStyle1{}

    }

    Action {
        id :addbookmark
        onTriggered: {
            mastercatalog.addBookmark(currentFolder)
            bookmarkadded()
        }
    }
    Controls.ActionButton{
        id : addBookmarkButton
        anchors.right : createCatalogForm.right
        anchors.bottom: createCatalogForm.bottom
        anchors.rightMargin: 5
        anchors.bottomMargin: 5
        buttontext : "Add Bookmark"
        iconsource : "../images/addbookmarkCS1.png"
        height : 0
        width :115
        action : addbookmark
        enabled : false
        z : 1
    }


    states: [
        State { name: "maximized"

            PropertyChanges {
                target: createCatalogForm
                height : 400
                opacity : 1

            }
            PropertyChanges {
                target: addBookmarkButton
                height : 40
            }
        },
        State {
            name : "minimized"
            PropertyChanges {
                target: createCatalogForm
                height : 0
                opacity : 0
            }
            PropertyChanges {
                target: addBookmarkButton
                height : 0
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
