import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global


Item {
    id : analysisv
    width : parent.width
    height : parent.height
    property string iconName : "../images/analysis"

    signal message(string msg)

    function refreshFormArea(form) {
        formArea.source = uicontext.ilwisFolder + "/extensions/ui/" + form
    }

    Component.onCompleted: {
        console.log("ManagerAnalysisView.qml: generating initial message...")
        message("sending this message")
    }

    SplitView {
        id : layerprops
        width : parent.width - 5
        height : parent.height

        function refreshAnalysisNames(){
            firstColumn.refreshList();
        }


        y : 2
        AnalysisList {
            id : firstColumn
            width : 320
            height: parent.height
        }
        Item {
            id : rest
            width:100
            Layout.fillWidth: true
            height : parent.height
            Loader {
                id : formArea
                anchors.fill: parent
            }
        }
        AnalysisCreateForm {
            id : createForm
            state : "invisible"
            height : parent.height
        }
    }
}
