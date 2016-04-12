import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../../../controls" as Controls

Item {
    height:parent.height
    width : 440

    function enable(propertyIndex, parms){
        visible = enabled = !visible
    }

    function getScriptText() {
        var wf = model.workflow("first_workflow_model") // atm we assume one workflow per scenario
        if ( wf !== null){
            return wf.generateScript("python",workflow.parameters())
        }
        return "";
    }

    TextArea{
        id : scripttext
        width : parent.width
        height : 150
        text: getScriptText()
    }

}
