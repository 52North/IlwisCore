import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global
import "../../../workbench/propertyform" as MetaData

Rectangle {
    id : meta
    width : parent.width
    height : parent.height
    color: "white"
    property int commonWidth : 530
    Controls.TextEditLabelPair{
        x : 5
        y : 4
        id : label
        labelText: qsTr("Label")
        labelWidth: 80
        width : commonWidth
        content : getLabel()
        onContentChanged: {
            if (workflowView.currentItem){
                if ( workflowView.currentItem.type !== "flowconnection"){
                    workflowView.workflow.setNodeProperty(workflowView.currentItem.itemid,-1,"label", content)
                    workflowView.currentItem.setLabel(content)
                }else{
                    var flow = workflowView.currentItem
                    workflowView.workflow.setNodeProperty(flow.target.itemid, flow.flowPoints.fromParameterIndex,"flowlabel", content)
                    workflowView.refreshCanvas()
                }
            }
        }
    }
    Column {
        id : parameterLabels
        width: parent.width
        anchors.top: label.bottom
        anchors.topMargin: 4
        height : (workflowView.currentItem && workflowView.currentItem.type) === "operationitem" ? 130 : 0
        enabled: height > 0
        visible: height > 0
        Text { id : plabel; text : qsTr("Input Parameter labels"); height : 22; width : 200; font.bold: true; visible : (workflowView.currentItem && workflowView.currentItem.type) === "operationitem"}
        ScrollView {
            width: commonWidth
            height : 100

            ListView{
                anchors.fill: parent
                model : (workflowView.currentItem && workflowView.currentItem.type === "operationitem")? workflowView.currentItem.getInputNames() : null
                delegate: Item {
                    width: commonWidth -17
                    height : 20
                    Controls.TextEditLabelPair{
                        x : 3
                        labelText: index
                        content: modelData
                        labelWidth: 80
                        width : parent.width
                        height: parent.height

                        onContentChanged: {
                            workflowView.workflow.updateOperationParameters(workflowView.currentItem.itemid, index, content)
                            if ( (workflowView.selectState && index == count) || !workflowView.selectState){
                                workflowView.currentItem.resetInputNames()
                                dataforms.updateSelectedForm()
                                dataforms.updateRunForm()
                            }
                        }
                    }
                }
            }
        }
    }

    Item {
        anchors.top: parameterLabels.bottom
        anchors.topMargin: 4
        width : commonWidth
        height :370
        x : 5
        y : 5
        ScrollView {
            anchors.fill: parent
            Controls.TextAreaLabelPair{
                width : commonWidth - 13
                height : (lineCount + 1) * 20

                id : txt
                labelText: qsTr("Comments")
                labelWidth: 80
                content: getText()
                readOnly: false
                onContentChanged: {
                    if (!workflowView.currentItem){
                        workflowView.workflow.description = content
                    } else if (workflowView.currentItem.type !== "flowconnection")
                        workflowView.workflow.setNodeProperty(workflowView.currentItem.itemid, -1, "description", content)
                    else {
                        var flow = workflowView.currentItem
                        workflowView.workflow.setNodeProperty(flow.target.itemid, flow.flowPoints.toParameterIndex, "description", content)

                    }
                }
            }
        }
    }
    function getLabel() {
        if (workflowView.currentItem){
            if ( workflowView.currentItem.type !== "flowconnection"){
                return workflowView.workflow.getNode(workflowView.currentItem.itemid).label
            } else{
                var flow = workflowView.currentItem
                var node = workflowView.workflow.getNode(flow.target.itemid)
                var txt = node["parameters"][flow.flowPoints.fromParameterIndex].flowlabel
                if ( !txt)
                    return "";
                return txt
            }
        }
        return ""
    }

    function getText(){
        if (!workflowView.currentItem){
            return workflowView.workflow.description
        }if ( workflowView.currentItem.type !== "flowconnection"){
            return workflowView.workflow.getNode(workflowView.currentItem.itemid).description
        }else {
            var flow = workflowView.currentItem
            var node = workflowView.workflow.getNode(flow.target.itemid)
            var txt = node["parameters"][flow.flowPoints.toParameterIndex].description
            if ( !txt)
                return "";
            return txt
        }
    }

    function selectedOperation() {
           txt.content = getText()
      }
}
