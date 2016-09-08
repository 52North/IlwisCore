import QtQuick 2.0

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UserMessageHandler 1.0
import MessageModel 1.0
import "./creators" as Create
import "../Global.js" as Global

WorkBenchShifter {
    id : creationContainer

    clip : true
    state : "invisible"
    height : 850

    FunctionBarHeader{
        id : functionBar
        headerImage: "../images/create.png"
        headerText:"Object Creation"
    }

    Create.ObjectList{
        id : createableObjects
        width : parent.width
        height : 150
        anchors.top : functionBar.bottom
        anchors.topMargin: 5
    }
    Create.CreatorList{
        id : editorList
        anchors.top : createableObjects.bottom
        anchors.topMargin: 5
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
    }

}

