import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import Qt.labs.folderlistmodel 2.1
import MasterCatalogModel 1.0
import "../Global.js" as Global

Rectangle {

    property string startFolder
    property string currentFolder

    function changeFoloder(url){
        if ( url !== folderModel.folder){
            folderModel.folder = url
            pathModel.fillModel()
        }

    }

    signal folderChanged(string newfolder)

    id : folderlist
    width : parent.width - 3
    height : parent.height  -3
    color : Global.mainbackgroundcolor
    border.width: 1
    border.color: Global.edgecolor
    radius : 5
    y : 3
    x : 0
    opacity : width > 4 ? 1 : 0

    function path2pathView(pathString){
        var index =  pathString.lastIndexOf("/..")
        if ( index !== -1){
            pathString = pathString.slice(0,index) // chop the /..
            index = pathString.lastIndexOf("/")
            if ( index !== -1 ){
                pathString = pathString.slice(0,index)  // chop the last 'real'folder as that is what we want here
            }
        }
        var found = false
        var i = 0
        for( ; i < pathModel.count; ++i){
            if ( pathModel.get(i).folderid === pathString.toLowerCase())    {
                found = true
                break;
            }
        }
        if (!found )
            pathModel.append({"folderid" : pathString})
        if ( privateProperty.pathCB != null){
            privateProperty.pathCB.currentIndex = i
        }
        return pathString
    }

    onCurrentFolderChanged:
        folderChanged(currentFolder)

    QtObject{
        id : privateProperty
        property ComboBox pathCB
    }


    ListModel {
           id: pathModel

           function fillModel() {
               var paths = mastercatalog.history
               var current =  mastercatalog.currentUrl
               current = Global.urlToLocalPath(current)
               pathModel.clear()
               for(var i=0; i < paths.length; ++i){
                   var path = Global.urlToLocalPath(paths[i])
                   pathModel.append({"folderid" : path})
                   if ( path === current){
                       pathText.currentIndex = i
                   }
               }
           }
       }


    ComboBox {
        id : drivelist
        anchors.top :folderlist.top
        width: parent.width
        height : 20
        model: mastercatalog.driveList()
        Layout.fillWidth: true
        onActivated: {
            currentIndex = index
            var drivePath = mastercatalog.getDrive(currentIndex)
            path2pathView(drivePath)
            folderModel.folder = Global.createfileUrlFromParts(drivePath, "")
            var filter = "container='" + folderModel.folder + "'"
            mainSplit.changeCatalog(filter,"catalog",folderModel.folder)
        }
    }
    Rectangle {
        id : pathView
        width: parent.width
        height : 20
        anchors.top : drivelist.bottom
        anchors.topMargin: 1
        color : Global.mainbackgroundcolor
        ComboBox {
            id : pathText
            width : parent.width - 26
            height : 18
            editable: true
            model: mastercatalog.history
            anchors.verticalCenter: parent.verticalCenter
            clip : true
            onCurrentIndexChanged: {
                if ( currentIndex == -1){
                    currentIndex = 0
                    editText = model[0]
                }
            }
            Component.onCompleted: {
                privateProperty.pathCB = pathText
                pathModel.fillModel()
            }
            onActivated: {
                currentIndex = index
                   var path = currentText
                   if ( path !== null && typeof path != 'undefined'){
                       folderModel.folder = currentText
                       var filter = "container='" + currentText+ "'"
                       mainSplit.changeCatalog(filter,"catalog",folderModel.folder)
                   }
            }
        }
        Button {
            width : 20
            height : 18
            anchors.right: parent.right
            anchors.rightMargin: 4
            anchors.verticalCenter: parent.verticalCenter
            text : "Go"
            action : goPath

        }
        Action {
            id : goPath
            onTriggered :{
                currentFolder = Global.createfileUrlFromParts(pathText.editText, "")
                folderModel.folder = currentFolder
                var filter = "container='" + folderModel.folder + "'"
                mainSplit.changeCatalog(filter,"catalog", currentFolder)
            }
        }

    }
    Rectangle {
        anchors.top : pathView.bottom
        anchors.topMargin: 3
        width: parent.width - 2;
        anchors.bottom : parent.bottom
        anchors.bottomMargin: 3
        clip : true
        x: 2
          radius : 5
        ListView {
            id : fileFolders
            anchors.fill: parent

            FolderListModel {
                id: folderModel
                showDirs: true
                showDotAndDotDot: true
                showFiles: false;

                Component.onCompleted: {
                    folder = mastercatalog.currentUrl
                }
            }

            model: folderModel
            delegate: Item {
                id: fileDelegate
                width : fileFolders.width
                height : 20

                Rectangle{
                    anchors.fill: parent
                    color :  fileFolders.currentIndex == index ? Global.alternatecolor1 : uicontext.workbenchBGColor
                    Image{ x : 2;y:3; id : folderico; source : "../images/data_folder.png"}
                    Text { id: filename; text: fileName; anchors.left : folderico.right; anchors.leftMargin: 5;anchors.verticalCenter: parent.verticalCenter }
                }

                MouseArea{
                    anchors.fill:  parent
                    hoverEnabled: true
                    onDoubleClicked: {
                        //fileFolders.currentIndex = index;
                        var path = folderModel.get(index,"filePath")
                        path = path2pathView(path)
                        if ( path.charAt(path.length - 1) === ':')
                            path += '/'
                        currentFolder = Global.createfileUrlFromParts(path, "")

                        folderModel.folder = currentFolder;
                        var filter = "container='" + folderModel.folder + "'"
                        mainSplit.changeCatalog(filter,"catalog", currentFolder)
                        pathModel.fillModel()
                    }

                }

            }
        }
    }

}
