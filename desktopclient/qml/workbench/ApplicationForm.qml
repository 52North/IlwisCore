import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick 2.0

Rectangle {
    property color background1 : "#EAECEE"
    property string formQML : ""
    property var currentAppForm : null
    property string formTitle : "est"

    id : applicationFormFrame
    width : parent.width
    height : parent.height
    radius : 10
    color : background1

    function doExecute(operationid) {
        if ( currentAppForm != null) {
            operations.executeoperation(operationid,currentAppForm.formresult)
        }
    }

    onFormQMLChanged: {
        if ( currentAppForm != null){
            currentAppForm.destroy(0)
        }

        if ( formQML.length !== 0) {
            currentAppForm = Qt.createQmlObject(formQML,
                applicationArea, "autoform1");
        }
    }

    Column {
        anchors.fill: parent
        spacing : 3
        BorderImage {
            id : title
            width: parent.width
            height : 25
            source : "../images/headerblue2CS1.png"
            border { left: 15; top: 0; right: 15; bottom: 0 }
            smooth : true
            Text  {
                text : formTitle
                font.pointSize: 11
                x : 5
            }
        }
        Rectangle {
            id : applicationArea
            property string exprparameters
            width : parent.width
            height : parent.height - applicationFormFrame.height - 20
        }
    }
}
