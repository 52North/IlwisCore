import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0

Item {
    height:parent.height
    width : 440

    function enable(propertyIndex, parms){
        visible = enabled = true
    }

    Button {
        width : 100
        height : 20
        text : "Debug"

        onClicked: {
            var wf = model.workflow("first_workflow_model") // atm we assume one workflow per scenario
            if ( wf !== null){
                wf.generateScript("python",workflow.parameters())
            }
        }
    }
}
