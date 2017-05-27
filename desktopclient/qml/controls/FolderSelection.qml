import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import Qt.labs.folderlistmodel 2.1
import MasterCatalogModel 1.0
import "../Global.js" as Global
import "../controls" as Controls
import PreferencesModel 1.0


Column {
    property string startFolder
    property string currentFolder
    property alias labelText : folderPath.labelText
    property alias labelWidth : folderPath.labelWidth

    signal folderChanged(string newfolder)
    signal resultFolder(string folder);

    onCurrentFolderChanged:
        folderChanged(currentFolder)

    function path2pathView(pathString){
        var index =  pathString.lastIndexOf("/..")
        if ( index !== -1){
            pathString = pathString.slice(0,index) // chop the /..
            index = pathString.lastIndexOf("/")
            if ( index !== -1){
                pathString = pathString.slice(0,index)  // chop the last 'real'folder as that is what we want here
            }
        }

        return pathString
    }
    onStateChanged: {
        if ( state === "minimized")
            folderlist.state = "minimized"
    }

    width : parent.width
    Row {
        width : parent.width
        height : 20
        Controls.TextEditLabelPair {
            id : folderPath
            width : parent.width
            content: currentFolder

        }
        Rectangle {
            height : Math.max(20, folderlist.height + 20)
            width : 20
            color : Global.alternatecolor4
            Image {
                id : marker
                height : 16
                width : 16
                anchors.centerIn: parent
                source : "../images/splithandle.png"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if ( folderlist.state == "minimized"){
                        marker.rotation = 90
                        folderlist.state = "maximized"
                        marker.source = "../images/splithandledark.png"
                    }else {
                        marker.rotation = 0
                        folderlist.state = "minimized"
                        marker.source = "../images/splithandle.png"
                        resultFolder(currentFolder)
                    }
                }
            }
        }

    }
    Rectangle {
        id : folderlist
        width : parent.width
        height : 0
        color : Global.mainbackgroundcolor
        border.width: 1
        border.color: Global.edgecolor
        radius : 5
        state : "minimized"
        y : 3
        x : 0
        opacity : width > 4 ? 1 : 0
        clip : true


        ComboBox {
            id : drivelist
            anchors.top :folderlist.top
            width: parent.width
            height : 20
            model: mastercatalog.driveList()
            Layout.fillWidth: true
            visible: Qt.platform.os === "windows" ? true : false
            onActivated: {
                currentIndex = index
                var drivePath = mastercatalog.getDrive(currentIndex)
                path2pathView(drivePath)
                currentFolder = Global.createfileUrlFromParts(drivePath, "")
                folderModel.folder = currentFolder;

            }
        }
        Rectangle {
            anchors.top : drivelist.bottom
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
                    folder : startFolder
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
                        color :  fileFolders.currentIndex == index ? Global.alternatecolor1 : uicontext.workbenchBGColor
                        Image{ x : 2;y:3; id : folderico; source : "../images/folder16.png"}
                        Text { id: filename; text: fileName; anchors.left : folderico.right; anchors.leftMargin: 5;anchors.verticalCenter: parent.verticalCenter }
                    }

                    MouseArea{
                        anchors.fill:  parent
                        hoverEnabled: true
                        onDoubleClicked: {
                            var path = folderModel.get(index,"filePath")
                            path = path2pathView(path)
                            currentFolder =  Global.createfileUrlFromParts(path, "");
                            folderModel.folder = currentFolder;
                        }

                    }

                }
            }
        }
        states: [
            State { name: "maximized"

                PropertyChanges {
                    target: folderlist
                    height : 250
                    opacity : 1

                }
            },
            State {
                name : "minimized"
                PropertyChanges {
                    target: folderlist
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
    states: [
        State { name: "maximized"

            PropertyChanges {
                target: locationsPane
                height : parent.height
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


