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
            updateView()
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

