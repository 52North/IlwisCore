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

Rectangle {
    id : operationForm
    width : 210
    height : parent ? parent.height : 0
    visible : height > 0
    property var operationid
    property int itemId: -1
    property bool saveButtonEnabled: itemId != -1 || conditionIds.length != 0
    property string conditionIds: ''


    function enable(propertyIndex, parms){
        visible = enabled = true
        showForm(propertyIndex,parms)
    }

    /**
    Create a form for the workflow
    */
    function showRunForm(parms) {
        var validValues = [], parameterindex, action;
        if (operationFormScrollView.appForm.currentAppForm != null ) {
            validValues = operationFormScrollView.appForm.currentAppForm.formresult.split('|')
            for (var i = 0; i < (parms.parameterIndexes && parms.parameterIndexes.length); i++) {
                parameterindex = parms.parameterIndexes[i].split('|');
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
        var form = formbuilder.index2Form(parms.workflowid, true, false, [], parms.operationNames, validValues)
        operationid = parms.workflowid

        operationFormScrollView.appForm.formQML = form
        operationFormScrollView.appForm.formTitle = qsTr("Set run values for workflow")
        operationFormScrollView.appForm.opacity = 1
        saveButtonEnabled = false
        conditionIds = ""
        //canvas.workflow.resetParameterEntrySet()

    }

      function showForm(propertyIndex, parms)  {
          var operationForm = parms ? parms.type !== "runform" : (propertyIndex !== 0)
          if ( operationForm ){
              if ( parms){
                  showOperationForm(parms)
              }else if ( parms === null && oldOperationFormParms){
                  showOperationForm(oldOperationFormParms)
              }
              if ( parms)
                  oldOperationFormParms = parms
          }
          else{
              if ( parms){
                  showRunForm(parms)
              }else if ( parms === null && oldRunFormParms){
                  showRunForm(oldRunFormParms)
              }
              if ( parms){
                  oldRunFormParms = parms
              }
          }
      }

    /**
      Shows the operation's form. Passes the hidden fields to the index2Form method.
      */
    function showOperationForm(parms){
        fillAppFrame(parms.item.operation.id, parms.operation + "(" + parms.item.getTitle() + ")", false, true, parms.hiddenFields, parms.constantValues)
        itemId = parms.operation
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

    function asignConstantInputData() {
        if (itemId > -1){
            canvas.assignConstantInputData(operationFormScrollView.appForm.currentAppForm.formresult, itemId)
        } else if (conditionIds.length > 0) {
            canvas.assignConditionInputData(operationFormScrollView.appForm.currentAppForm.formresult, conditionIds);
        }
    }

    BorderImage {
        id : title
        width: parent.width
        height : 25
        opacity : 1
        anchors.bottomMargin: operationForm.formTitle != "" ? 20 : 0
        source : "../../../images/headerblue2CS1.png"
        border { left: 15; top: 0; right: 15; bottom: 0 }
        smooth : true
        Text {
            text : operationFormScrollView.appForm.formTitle
            font.pointSize: 11
            x : 5
        }
    }
    Item {
        id: form
        width : 440
        height : saveButtonEnabled ? parent.height - 60 : parent.height - 30
        anchors.top: title.bottom
        anchors.topMargin: 3

        Forms.OperationForm{
            id : operationFormScrollView
            container : form
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

}

