import QtQuick 2.0
import ".." as Model

Model.ModellerWorkArea {
    property var currentApplication

    onCurrentApplicationChanged: {
        updateView()
    }

    width : parent.width
    height : parent.height - 170
    color : "#F0FFFF"
    state : "invisible"

    onStateChanged: {
        if ( state == "visible"){
            if ( !meme.item)
                updateView()
            else{
               meme.item.visible = true
                meme.item.enabled = true
            }
        }else if ( state == "invisible"){
            if ( meme.item){
                meme.item.visible = false
                meme.item.enabled = false
            }
        }
    }

    Loader {
        anchors.fill: parent
        id: meme
    }

    function updateView() {
        meme.source = currentApplication ? (uicontext.ilwisFolder + "/extensions/ui/" + currentApplication.panel("main")) : ""
        state = "visible"
    }
}

