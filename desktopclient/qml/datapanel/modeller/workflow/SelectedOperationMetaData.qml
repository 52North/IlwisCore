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
    width : 464
    height : parent.height
    color: "white"
    ScrollView {
        x : 5
        y : 5
        width : 600
        height : meta.height

        Controls.TextAreaLabelPair{
            width : 600
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
    function getText(){
        if (!workflowView.currentItem){
            return workflowView.workflow.description
        }if ( workflowView.currentItem.type !== "flowconnection"){
            return workflowView.workflow.getNode(workflowView.currentItem.itemid).description
        }else {
            var flow = workflowView.currentItem
            var node = workflowView.workflow.getNode(flow.target.itemid)
            var txt = node["parameters"][flow.flowPoints.toParameterIndex].description
            return txt
        }
    }

    function selectedOperation() {
           txt.content = getText()
      }
}
