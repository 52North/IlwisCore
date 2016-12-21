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

    property alias loaderItem: formArea.item

    signal msg(string msg)

    function refreshFormArea(form) {
        formArea.source = uicontext.ilwisFolder + "/extensions/ui/" + form
    }

    function sendthemessagetest() {
        //console.log("ManagerAnalysisView.qml: generating initial message...")
        //msg("sending this message")
    }

    // todo: decouple these functions...
    function smcemodeChanged(msg) {
        console.log("ManagerAnalysisView.qml: smcemodeChanged: " + msg)
        formArea.item.smcemodeChanged(msg)
    }

    function updateNodeInfo(msg) {
        console.log("ManagerAnalysisView.qml: updateNodeInfo was called with: " + msg)
        formArea.item.updateNodeInfo(msg)
    }

    function setFields(msg) {
        console.log("ManagerAnalysisView.qml: setFields was called with: " + msg)
        formArea.item.setFields(msg)
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
