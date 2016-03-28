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
    property var oldOperationFormParms
    property var oldRunFormParms

    function enable(propertyIndex, parms){
        workflow.visible = operation.visible = metadata.visible = false
        workflow.enabled = operation.enabled = metadata.enabled = false
        if ( propertyIndex === 0)
            workflow.enable(propertyIndex,parms);
        if ( propertyIndex === 1)
            operation.enable(propertyIndex,parms)
        if ( propertyIndex === 2)
            metadata.enable(propertyIndex,parms)

    }

    OperationForms{
        id : workflow
        height : parent.height - 30
        visible: false
        enabled: false

        RunForm {
            width : parent.width
            height : 25
            anchors.top : workflow.bottom

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
}
