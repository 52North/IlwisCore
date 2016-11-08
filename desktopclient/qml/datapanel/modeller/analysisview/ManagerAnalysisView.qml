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

    SplitView {
        id : layerprops
        width : parent.width - 5
        height : parent.height
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

        }
        AnalysisCreateForm {
            id : createForm
            state : "invisible"
            height : parent.height
        }
    }



}
