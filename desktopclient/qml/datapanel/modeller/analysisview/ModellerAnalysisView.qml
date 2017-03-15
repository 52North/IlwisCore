import QtQuick 2.0
import UIContextModel 1.0
import ".." as Model

Model.ModellerWorkArea {

    property var currentAnalysis

    onCurrentAnalysisChanged: {
        updateView()
    }

    width : parent.width
    height : parent.height - 170

    color : "white"
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
        onLoaded :{
            item.visible = true
        }

        id: meme
    }

    function updateView() {
        meme.source = currentAnalysis ? (uicontext.ilwisFolder + "/extensions/ui/" + currentAnalysis.panel("main")) : ""
        state = "visible"
    }
}

