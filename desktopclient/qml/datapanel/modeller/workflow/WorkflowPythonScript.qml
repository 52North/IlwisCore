import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../../controls" as Controls

Item {
    height:parent.height
    width : 250

    function enable(propertyIndex, parms){
        visible = enabled = !visible
    }

    function getScriptText() {
        if ( modellerDataPane.model){
            var wf =  modellerDataPane.workflowModel()
            return wf.generateScript("python",dataforms.parameters())
        }
        return "";
    }

    TextArea{
        id : scripttext
        width : parent.width
        height : parent.height - 10
        text: getScriptText()
    }
}
