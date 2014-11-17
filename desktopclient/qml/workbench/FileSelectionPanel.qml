import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import Qt.labs.folderlistmodel 2.1
import MasterCatalogModel 1.0

Rectangle {

    property string startFolder
    property string currentFolder

    signal folderChanged(string newfolder)

    id : folderlist
    width : parent.width - 3
    height : parent.height - 30
    color : "white"
    radius: 10
    y : 3
    x : 0

    function path2pathView(pathString){
        var index =  pathString.lastIndexOf("/..")
        if ( index !== -1){
            pathString = pathString.slice(0,index) // chop the /..
            index = pathString.lastIndexOf("/")
            if ( index !== -1){
                pathString = pathString.slice(0,index)  // chop the last 'real'folder as that is what we want here
            }
        }

        var found = false
        var i =0
        for( ; i < pathModel.count; ++i){
            if ( pathModel.get(i).folderid === pathString)    {
                found = true
                break;
            }
        }
        if (!found)
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
    }


    ComboBox {
        id : drivelist
        anchors.top :folderlist.top
        width: parent.width
        height : 20
        model: mastercatalog.driveList()
        Layout.fillWidth: true
        onCurrentIndexChanged: {
            var drivePath = mastercatalog.getDrive(currentIndex)
            path2pathView(drivePath)
            folderModel.folder = "file:///"+ drivePath
            mastercatalog.setWorkingCatalog(folderModel.folder)
        }
        Component.onCompleted: {
            mastercatalog.addCatalog(startFolder) // at this place else the indexchanged will overrule any previous path setting
            mastercatalog.setWorkingCatalog(startFolder)

        }
    }
    Rectangle {
        id : pathView
        width: parent.width
        height : 20
        anchors.top : drivelist.bottom
        anchors.topMargin: 1
        border.width: 1
        border.color: "lightgrey"

        ComboBox {
            id : pathText
            width : parent.width - 26
            height : 18
            editable: true
            model: pathModel
            anchors.verticalCenter: parent.verticalCenter
            onCurrentIndexChanged: {
                var path = pathModel.get(currentIndex)
                if ( path !== null && typeof path != 'undefined'){
                    folderModel.folder = "file:///" + path.folderid
                    mastercatalog.addCatalog(folderModel.folder)
                    mastercatalog.setWorkingCatalog(folderModel.folder)
                }
            }
            Component.onCompleted: {
                privateProperty.pathCB = pathText
                var paths = mastercatalog.knownCatalogs(true)
                for(var i=0; i < paths.length; ++i){
                    pathModel.append({"folderid" : paths[i].replace("file:///","")})
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
                currentFolder = "file:///"+ pathText.editText
                folderModel.folder = currentFolder
                mastercatalog.addCatalog(currentFolder)
                mastercatalog.setWorkingCatalog(currentFolder)
            }
        }

    }
    Rectangle {
        anchors.top : pathView.bottom
        anchors.topMargin: 3
        width: parent.width;
        height :parent.height - 5
        clip : true
        ListView {
            id : fileFolders
            anchors.fill: parent

            FolderListModel {
                id: folderModel
                folder : startfolder
                showDirs: true
                showDotAndDotDot: true
                showFiles: false;
            }

            model: folderModel
            delegate: Item {
                id: fileDelegate
                width : fileFolders.width
                height : 20

                Rectangle{
                    anchors.fill: parent
                    color :  fileFolders.currentIndex == index ? "lightgrey" : "white"
                    Image{ x : 2;y:3; id : folderico; source : "../images/folder16.png"}
                    Text { id: filename; text: fileName; anchors.left : folderico.right; anchors.leftMargin: 5;anchors.verticalCenter: parent.verticalCenter }
                }

                MouseArea{
                    anchors.fill:  parent
                    hoverEnabled: true
                    onClicked: {
                        //fileFolders.currentIndex = index;
                        var path = folderModel.get(index,"filePath")
                        mastercatalog.addCatalog(path);
                        path = path2pathView(path)
                        currentFolder = "file:///"+ path;
                        folderModel.folder = currentFolder;
                        mastercatalog.setWorkingCatalog(currentFolder)
                    }

                }

            }
        }
    }

}
