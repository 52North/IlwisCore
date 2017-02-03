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
                Row {
                    width : parent.width
                    height : 25
                    ComboBox{
                        width : 45
                        height : parent.height
                        model: [" ", "not"]
                    }

                    TextField{
                        id : testText
                        width : parent.width - 115
                        height : parent.height
                        readOnly: true
                        text : modelData.name2
                    }


                    Button {
                        width : 25
                        height : 25
                        Image {
                            width : 20
                            height : 20
                            anchors.centerIn: parent
                            source : "../../../images/zoomin20.png"
                        }
                        onClicked: {
                            if ( testDetails.height == 0){
                                testDetails.model = null
                                testDetails.model = workflow.getTestParameters(itemid, index)
                                detailsBack.height = testDetails.model.length * 20
                                testList.currentIndex = index
                            }else{
                                for(var t=0; t < detailsBack.values.length; ++t)
                                    workflow.setTestValues(itemid, testList.currentIndex, t, detailsBack.values[t])
                                conditionItem.resetColors()
                                setTests()

                                detailsBack.values = []
                                testDetails.model = null
                                detailsBack.height = 0

                            }
                        }
                    }

                    ComboBox{
                        width : 45
                        height : parent.height
                        model: [" ", "and", "or"]
                    }
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
                                anchors.right : parmvalues.left
                                anchors.rightMargin: 15
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

                            Controls.TextEditLabelPair{
                                id : parmvalues
                                width : Math.max(0,parent.width - 20)
                                labelWidth: 160
                                labelText: modelData.label
                                height : 20
                                content : modelData.value
                                onContentChanged: {
                                    detailsBack.values[index] = content
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

    function addTestOperation(operationid){
        var operation = operations.operation(operationid)
        if ( operation && operation.booleanOperation){
            addTest(-1, "", operation,"","","")
        }
    }
}

