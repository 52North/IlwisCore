import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import OperationModel 1.0
import MasterCatalogModel 1.0
import "../../Global.js" as Global


Rectangle {
    id : operationProps
    property OperationModel operationmd
    color : uicontext.lightestColor
    Column {
        id : simpleProps
        width : parent.width
        Row {
            height : Global.rowHeight
            width : parent.width
            Text{
                id : label1
                text : qsTr("Description")
                width : 60
                font.bold : true
            }

            Text{
                width : parent.width - label1.width
                height : Global.rowHeight
                id : desc
            }
        }
        Row {
            height : Global.rowHeight
            width : parent.width
            Text{
                id : label2
                text : qsTr("Definition")
                width : 60
                font.bold : true
            }
            Text {
                id : syntax
                width : parent.width - label2.width
                height : Global.rowHeight
            }
        }
        Row {
            height : Global.rowHeight
            width : parent.width
            Text{
                id : label3
                text : qsTr("Keywords")
                width : 60
                font.bold : true
            }
            Text {
                id : keywords
                width : parent.width - label2.width
                height : Global.rowHeight
            }
        }

    }

     Text {
        anchors.top : simpleProps.bottom
        id : inputLabel
        text : qsTr("Input parameters (") + getProperty("inparameters") + ")"
    }
    Rectangle {
        id : inputParams
        anchors.top : inputLabel.bottom
        width : parent.width
        height : 110
        border.width : 1
        border.color : Global.edgecolor
        ScrollView {
            anchors.fill: parent
            anchors.margins: 1
            ListView {
                anchors.fill: parent
                id : inputList
                clip : true
                delegate : ParameterDelegate{
                    parmnam: operationmd ? operationmd.inputparameterName(index) : ""
                    parmtype: operationmd ? operationmd.inputparameterTypeNames(index) : ""
                    parmdesc: operationmd ? operationmd.inputparameterDescription(index) : ""
                }
            }
        }
    }
    Text {
        anchors.top : inputParams.bottom
        id : outputLabel
        text : qsTr("Output parameters (") + getProperty("outparameters") + ")"
    }
    Rectangle {
        id : outputParams
        anchors.top : outputLabel.bottom
        width : parent.width
        height : 30
        border.width : 1
        border.color : Global.edgecolor
        ScrollView {
            anchors.fill: parent
            anchors.margins: 1
            ListView {
                anchors.fill: parent
                id : outputList
                clip : true
                delegate : ParameterDelegate{
                    parmnam: operationmd ? operationmd.outputparameterName(index) : ""
                    parmtype: operationmd ? operationmd.outputparameterTypeNames(index) : ""
                    parmdesc: operationmd ? operationmd.outputparameterDescription(index) : ""
                }
            }
        }
    }

    Component.onCompleted: {
        operationmd = operation(id)
        if ( operationmd){
            desc.text = operationmd.description
            syntax.text = operationmd.syntax
            keywords.text = operationmd.keywords;
            inputList.model = operationmd.inParamNames
            outputList.model = operationmd.outParamNames
        }
    }
}

