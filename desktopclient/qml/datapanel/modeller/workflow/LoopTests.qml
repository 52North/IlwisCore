import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Rectangle {
    id: testRectangle
    property int detailsHeight : 0
    property alias currentTestIndex : testList.currentIndex

    border.width: 1
    border.color: "grey"
    color: workflow.isValidNode(itemid,"tests") ? testColor : Global.errorColor
    height : 35 + detailsHeight
    width : parent.width - 8
    x : 4
    y: 4
    radius : 5
    z : 10

    Component  {
        id : testTextField

        Controls.TextEditLabelPair{
            property var values : []
            property var modelData
            property int index

            id : parmvalues
            //width : Math.max(0,parent.width - 20)
            labelWidth: 160
            labelText: modelData.label
            height : 20
            content : modelData.value
            onContentChanged: {
                parmvalues.values[parmvalues.index] = content
            }

        }
    }

    Component  {
        id : testRangeField

        Controls.ComboxLabelPair{
             property var values : []
            property var modelData
            property var choices
            property int index


            id : parmvalues
            //width : Math.max(0,parent.width - 20)
            labelWidth: 160
            labelText: modelData.label
            height : 20
            content : modelData.value
            itemModel: choices
            onContentChanged: {
                parmvalues.values[parmvalues.index] = content
            }
        }
    }

     DropArea {
        x : 2
        y : 2
        z: 1

        height : parent.height
        width : parent.width - 4
        enabled : true

        Text {
           anchors.centerIn: parent
           text : qsTr("Drop test operation(s) here")
           color : "grey"
        }
        onDropped: {
            addTestOperation(drag.source.ilwisobjectid)
        }

        ListView{
            id :testList
            property var tests : []
            width : parent.width - 28
            height : parent.height
            z : 2
            delegate: Column {
                id : testPart
                width : parent.width
                height : 25

                LoopTest{
                    width : parent.width
                    db : detailsBack
                    td : testDetails
                }

                Rectangle {
                    id : detailsBack
                    width : parent.width
                    height : 0
                    property var values : []
                    ListView {
                        id : testDetails
                        anchors.fill: parent
                        onModelChanged: {
                            detailsBack.height = model ? model.length * 20 : 0
                            testRectangle.detailsHeight = detailsBack.height
                        }

                        delegate : Item{
                            id : parmRow
                            width : parent.width
                            height : 20
                            Rectangle{
                                id : attachDetail
                                x : -15
                                width : 10
                                height : 10
                                border.width: 1
                                MouseArea{
                                    anchors.fill : parent
                                    onClicked: {
                                        testDetails.currentIndex = index
                                        var datatype = workflow.testValueDataType(itemid, testList.currentIndex, index)
                                        attachTestFlow(type,index )
                                        att1.finishFlow(0,index)
                                        testRectangle.detailsHeight = 0
                                        testRectangle.height = 35
                                    }
                                }

                            }
                            Loader {
                                id : controlfield
                                width : parent.width
                                height : 20
                                sourceComponent: modelData.choicelist.length > 0 ? testRangeField : testTextField
                                onLoaded: {
                                    controlfield.item.width = Math.max(0,parent.width - 20)
                                    controlfield.item.modelData = modelData
                                    controlfield.item.values = detailsBack.values
                                    controlfield.item.index = index
                                    if(modelData.choicelist.length > 0)
                                        controlfield.item.choices = modelData.choicelist
                                }
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            width : 22
            height : 22
            anchors.left: testList.right
            anchors.leftMargin: 3
            y : 2
            Image{
                width : 20
                height : 20
                source : icon
                anchors.centerIn: parent
            }
        }
    }

    function addTestOperation(operationid){
        var operation = operations.operation(operationid)
        if ( operation && operation.booleanOperation){
            workflow.addTest2Condition(itemid,operation.id,"", "")
            testList.model = null
            testList.model = workflow.getTests(itemid)
        }else if ( operation.name === "range"){
            workflow.addTest2Condition(itemid,operation.id,"", "")
            testList.model = null
            testList.model = workflow.getTests(itemid)
        }
    }

    function setTests() {
        testList.model = workflow.getTests(itemid)
    }

    function addTest(index, pre, operation, value, post,type) {
        if ( index === -1)
            index = testList.tests.length
        workflow.addTest2Condition(itemid,operation.id,pre, post)
        testList.model = null
        testList.model = workflow.getTests(itemid)
    }

    function setValue(operationItemid, fromParam, toParam){
        var value = "link=" + operationItemid + ":" +fromParam
        workflow.setTestValues(itemid, testList.currentIndex, toParam, value)
        testList.model = null
        testList.model = workflow.getTests(itemid)
    }
}


