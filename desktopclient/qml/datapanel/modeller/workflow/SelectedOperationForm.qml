import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../../workbench" as Bench
import "../../../Global.js" as Global

Rectangle  {
    width : 600
    height : parent.height
    color : "white"
    ScrollView{
        id: operationFormScrollView
        property var container
        width : parent.width
        property alias appForm : appFrame

        Bench.ApplicationForm{
            id : appFrame
            y : 20
            width : operationFormScrollView.width - 30
            height : parent.height
            showTitle: true
        }
    }

    Button {
        anchors.bottom : parent.bottom
        anchors.bottomMargin: 4
        width : 120
        height : 22
        text : qsTr("Set Fixed vValues")
        onClicked: {
            var currentOperation = workflowView.currentOperation()
            if ( currentOperation){
                workflowView.workflow.setFixedValues(currentOperation.itemid, appFrame.currentAppForm.formresult)
                workflowView.workflow.createMetadata()
                currentOperation.resetColors()
                workflowView.updateInputNamesList()
            }
        }
    }

    function fillAppFrame() {
        var currentOperation = workflowView.currentOperation()
        if (!currentOperation)
            return
        var node = workflowView.workflow.getNode(currentOperation.itemid)
        if (!node)
            return
        var constantValues = []
        var parms = node.parameters
        if ( parms){
            for(var i=0; i < parms.length; ++i){
                var parm = parms[i]
                constantValues.push(parm.state === 'fixed' ? parm.value : "")
            }

            var form = formbuilder.index2Form(currentOperation.operation.id, false, true, {}, {}, constantValues)
            appFrame.formQML = ""
            appFrame.formQML = form
            appFrame.formTitle = currentOperation.operation.displayName
            appFrame.opacity = 1
        }
    }

}
