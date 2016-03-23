import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../workbench" as Bench
import "../../workbench/propertyform" as PropertyForm
import "../../Global.js" as Global
import "workflow/forms" as Forms

Rectangle {
    id : operationForm
    width : 210
    height : parent.height
    visible : height > 0
    property var operationid
    property int itemId: -1
    property bool saveButtonEnabled: itemId != -1 || conditionIds.length != 0
    property string conditionIds: ''

    /**
    Create a form for the workflow
    */
    function showRunForm(metaid, operationNames, parameterIndexes) {
        var validValues = [], parameterindex, action;

        if (operationFormScrollView.appForm.currentAppForm != null) {
            validValues = operationFormScrollView.appForm.currentAppForm.formresult.split('|')
            for (var i = 0; i < parameterIndexes.length; i++) {
                parameterindex = parameterIndexes[i].split('|');
                action = parameterindex[1];
                parameterindex = parameterindex[0];

                if (action === 'insert') {
                    validValues.splice(parameterindex, 0, "")
                } else if (action === 'remove') {
                    validValues.splice(parameterindex, 1)
                }
            }
            validValues = validValues.slice(0, canvas.workflow.getInputParameterCount())
        }
        var form = formbuilder.index2Form(metaid, true, false, [], operationNames, validValues)
        operationid = metaid

        operationFormScrollView.appForm.formQML = form
        operationFormScrollView.appForm.formTitle = qsTr("Set run values for workflow")
        operationFormScrollView.appForm.opacity = 1
        saveButtonEnabled = false
        conditionIds = ""
        //canvas.workflow.resetParameterEntrySet()

    }

    function showForm(item, title, newItemId, constantValues){
        fillAppFrame(item.operation.id, title + "(" + item.getTitle() + ")", false, true, "", constantValues)
        itemId = newItemId
        saveButtonEnabled = true
    }

    /**
      Shows the operation's form. Passes the hidden fields to the index2Form method.
      */
    function showOperationFormWithHiddenFields(item, title, newItemId, constantValues, hiddenFields){
        fillAppFrame(item.operation.id, title + "(" + item.getTitle() + ")", false, true, hiddenFields, constantValues)
        itemId = newItemId
        saveButtonEnabled = true
    }

    /**
      Shows the operation's form. Passes the hidden fields to the index2Form method.
      */
    function showConditionForm(operationId, hiddenFields, constantValues, ids){
        fillAppFrame(operationId, "Condition form", false, true, hiddenFields, constantValues)
        itemId = -1
        saveButtonEnabled = true
        conditionIds = ids
    }

    function fillAppFrame(metaid, title, output, showEmpty, hiddenFields, constantValues) {
        var form = formbuilder.index2Form(metaid, output, showEmpty, hiddenFields, {}, constantValues)
        operationid = metaid
        operationFormScrollView.appForm.formQML = ""
        operationFormScrollView.appForm.formQML = form
        operationFormScrollView.appForm.formTitle = title
        operationFormScrollView.appForm.opacity = 1
        canvas.workflow.setSelectedOperationId(metaid)
        props.model = canvas.workflow.selectedOperation

    }

    /**
      Executes the form
      */
    function executeForm() {
        operationFormScrollView.appForm.doExecute(operationid)
        return appFrame.currentAppForm.formresult
    }

    function clearOperationForm() {
        operationFormScrollView.appForm.formTitle = "Nothing selected"
        operationFormScrollView.appForm.formQML = ""
        itemId = -1
        saveButtonEnabled = false
        conditionIds = ""
    }

    BorderImage {
        id : title
        width: parent.width
        height : 25
        opacity : 1
        anchors.bottomMargin: operationForm.formTitle != "" ? 20 : 0
        source : "../../images/headerblue2CS1.png"
        border { left: 15; top: 0; right: 15; bottom: 0 }
        smooth : true
        Text {
            text : operationFormScrollView.appForm.formTitle
            font.pointSize: 11
            x : 5
        }
    }
    SplitView{
        width : parent.width
        height : saveButtonEnabled ? parent.height - 60 : parent.height - 30
        anchors.top: title.bottom
        anchors.topMargin: 3
        Item {
            id: form
            width: 440
            height: parent.height

            Forms.OperationForm{
                id : operationFormScrollView
                container : operationForm
                height : parent.height - saveConstantInputButton.height - 10
            }


            Forms.AppFormApplyButton{
                id : saveConstantInputButton
                height : saveButtonEnabled ? 25 : 0
                width : 70
                x: Math.min(350, operationFormScrollView.width - 60)
                anchors.top: operationFormScrollView.bottom
            }

        }

        Rectangle {
            height:parent.height
            ListView {
                id : props
                anchors.fill: parent
                property int lastIndex : 0
                delegate : PropertyForm.DPropertyForm{}
            }
        }
    }

    function asignConstantInputData() {
        if (itemId > -1){
            canvas.assignConstantInputData(appFrame.currentAppForm.formresult, itemId)
        } else if (conditionIds.length > 0) {
            canvas.assignConditionInputData(appFrame.currentAppForm.formresult, conditionIds);
        }
    }
}

