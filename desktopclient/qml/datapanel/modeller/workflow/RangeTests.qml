import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Rectangle {
    id: testRectangle

    border.width: 1
    border.color: "grey"
    color: "grey"//workflow.isValidNode(itemid,"tests") ? testColor : Global.errorColor
    height : 35
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

        onDropped: {
            addTestOperation(drag.source.type, drag.source.ilwisobjectid, drag.source.message)
        }
        Row {
            width : parent.width
            height : parent.height
            TextField {
                id : rangeDefinition
                width : parent.width - marker.width
                height : parent.height - 6
                y : 1
                z : 2
                //readOnly: true
            }


            Button {
                id : marker
                width : 28
                height : parent.height -6
                y : 1
                Image{
                    width : 20
                    height : 20
                    source : icon
                    anchors.centerIn: parent
                }
                onClicked: {
                    workflow.setRangeTestValues(rangeItem.itemid, rangeDefinition.text)
                }
            }
        }
    }
    function setTests() {
        var node = workflow.getNode(rangeItem.itemid)
        if ( node){
            rangeDefinition.text = node["rangedefinition"]
        }
    }

    function setTest(txt){
        rangeDefinition.text = txt;
    }


    function addTestOperation(objectid, url, typename){
            var object = mastercatalog.id2object(objectid,testRectangle)
        if (object)
            rangeDefinition.text = object.shortRangeDefinition()
    }

    function storeRangeDefinition() {
        console.debug("stored")
        workflow.setRangeTestValues(rangeItem.itemid, rangeDefinition.text)
    }
}
