import QtQuick 2.0
import "../../../../workbench/propertyform" as PropertyForm

Rectangle {
    height:parent.height
    property alias listmodel : props.model

    function enable(propertyIndex, parms){
        visible = enabled = true
        showForm(propertyIndex,parms)
    }

    function showForm(propertyIndex, parms){
        var wf = model.workflow("first_workflow_model") // atm we assume one workflow per scenario
        if ( wf !== null)
            listmodel = wf.selectedOperation
    }

    ListView {
        id : props
        anchors.fill: parent
        property int lastIndex : 0
        delegate : PropertyForm.DPropertyForm{}
    }

    Component.onCompleted: {
//        var wf = model.workflow("first_workflow_model") // atm we assume one workflow per scenario
//        if ( wf !== null)
//            listmodel = wf.selectedOperation
    }
}
