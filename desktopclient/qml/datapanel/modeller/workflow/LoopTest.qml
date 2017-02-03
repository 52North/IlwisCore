import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Row {
    height : 25
    property var db
    property var td
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
        text :modelData.name2
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
            setDetails(index)
        }
    }

    ComboBox{
        width : 45
        height : parent.height
        model: [" ", "and", "or"]
    }

    function setDetails(index) {
        if ( testDetails.height == 0){
            testDetails.model = null
            testDetails.model = workflow.getTestParameters(itemid, index)
            db.height = testDetails.model.length * 20
            testList.currentIndex = index
        }else{
            for(var t=0; t < detailsBack.values.length; ++t)
                workflow.setTestValues(itemid, testList.currentIndex, t, detailsBack.values[t])
            conditionItem.resetColors()
            setTests()

            testDetails.model = null
            db.height = 0
            db.values = []

        }
    }
}
