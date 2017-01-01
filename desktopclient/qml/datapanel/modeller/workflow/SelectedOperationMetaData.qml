import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global
import "../../../workbench/propertyform" as MetaData

ScrollView {
    width : 464
    height : parent.height

    ListView {
        id : props
        anchors.fill: parent
        property int lastIndex : 0
        model :  workflowView.workflow.selectedOperation
        delegate : MetaData.DPropertyForm{}
    }

    function selectedOperation() {
        var op = workflowView.currentOperation()
        if ( op){
            workflowView.workflow.selectOperation(op.operation.id)

        }
    }
}
