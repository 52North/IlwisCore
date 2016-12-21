import QtQuick 2.0
import UIContextModel 1.0
import ".." as Model

Model.ModellerWorkArea {

    property var currentAnalysis

    signal message(string msg)
    signal nodeChanged (string node)


    onCurrentAnalysisChanged: {
        updateView()
    }

    width : parent.width
    height : parent.height - 170
    color : "white"
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

    Component.onCompleted: {
        //console.log("ModellerAnalysisView.qml: generating initial message...")
        //message("sending this message")
    }

    function updateView() {
        meme.source = currentAnalysis ? (uicontext.ilwisFolder + "/extensions/ui/" + currentAnalysis.panel("main")) : ""
        state = "visible"
    }

    function sendMessage(msg) {
        console.log("ModellerAnalysisView.qml: received: " + msg)
    }
}

