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
    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name

        if ( name === "")
            name = "redbuttonr.png"

         var iconP = "../images/" + name
         return iconP
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
        height : parent.height - 15
        anchors.left: parent.left
        anchors.right : parent.right
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        clip : true
        Tab {
            id : filetab
            height: parent.height
            width : parent.width
            title : "Local"
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

            title : "Remote"
            height: parent.height
            width : parent.width
            Item {
                anchors.fill: parent
                anchors.topMargin: 5
                SelectServiceForm{}
            }
        }

        Tab {
            title : "Selection"
        }
        style: Base.TabStyle1{}

    }

    Component.onCompleted: {
        currentFolder =  mastercatalog.currentUrl
    }

    states: [
        State { name: "maximized"

            PropertyChanges {
                target: createCatalogForm
                height : 370
                opacity : 1

            }
        },
        State {
            name : "minimized"
            PropertyChanges {
                target: createCatalogForm
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
