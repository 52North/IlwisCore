import QtQuick 2.0
import UIContextModel 1.0
import ".." as Model

Model.ModellerWorkArea {

    property var currentAnalysis

    onCurrentAnalysisChanged: {
        meme.source = currentAnalysis ? (uicontext.ilwisFolder + "/extensions/ui/" + currentAnalysis.mainPanel) : ""
    }

    width : parent.width
    height : parent.height - 170
    color : "white"
    state : "invisible"


    Loader {
        anchors.fill: parent
        id: meme

    }

}

