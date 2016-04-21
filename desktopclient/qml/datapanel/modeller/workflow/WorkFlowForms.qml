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
import "forms" as Forms

Item {
    id : forms
    property var oldOperationFormParms
    property var oldRunFormParms

    function enable(propertyIndex, parms){
        workflow.visible = operation.visible = metadata.visible = script.visible = false
        workflow.enabled = operation.enabled = metadata.enabled = script.enabled = false
        if ( propertyIndex === 0)
            workflow.enable(propertyIndex,parms);
        if ( propertyIndex === 1)
            operation.enable(propertyIndex,parms)
        if ( propertyIndex === 2)
            metadata.enable(propertyIndex,parms)
        if ( propertyIndex === 3)
            script.enable(propertyIndex,parms)

    }

    function clearOperationForm() {
        operation.clearOperationForm()
    }

    OperationForms{
        id : workflow
        height : parent.height - 55
        visible: false
        enabled: false

        RunForm {
            id : runform
            width : parent.width
            height : 55
            anchors.top : workflow.bottom
        }
        StepForm{
            visible : workflow.visible & runform.stepMode
            enabled : workflow.enabled
            anchors.left: workflow.right
            anchors.right: forms.right
            height : parent.height
        }

    }

    OperationForms{
        id : operation
        visible: false
        height : parent.height
        enabled : false
    }

    Forms.OperationPropForm{
        id : metadata
        width : parent.width
        visible: false
        enabled: false

    }
    Forms.WorkflowPythonScript{
        id : script
        width : parent.width
        visible: false
        enabled: false

    }
}
