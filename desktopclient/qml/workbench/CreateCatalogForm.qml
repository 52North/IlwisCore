import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import Qt.labs.folderlistmodel 2.1
import MasterCatalogModel 1.0
import ".." as Base
import "../controls" as Controls
import "../Global.js" as Global

Item {
    height : 0
    width : 0
    opacity: 0
    objectName: "main_ui_catalogform"

    property string beginfolder
    property string currentFolder

    onCurrentFolderChanged: {
        if ( tabs){
            var tabitem = tabs.getTab(0)
            if ( tabitem){
                tabitem.item.changeFolder(currentFolder)
            }
        }
    }

    function newCurrentFolder(url){
        currentFolder = url;
    }

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

        property var tooltips : ["View on folder structure of the local computer",
            "Creates access to remote services by specifying their connection properties",
            "Not implemented yet"];

        Tab {
            id : filetab
            height: parent.height
            width : parent.width
            title : qsTr("Local")

            Item {
                function changeFolder(url){
                   fileselection.changeFoloder(url)
                }
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

            title : qsTr("Remote")
            height: parent.height
            width : parent.width
            Item {
                anchors.fill: parent
                anchors.topMargin: 5
                SelectServiceForm{}
            }
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
