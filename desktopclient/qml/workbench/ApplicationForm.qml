import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick 2.0

Rectangle {
    property color background1 : "white"
    property string formQML : ""
    property var formComponent : null
    property var currentAppForm : null
    property string formTitle
    property string operationId : "id"
    property string operationUrl : "url"

    signal formResultChanged();

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
            background1 = formTitle == "" ? "transparent" : "white"
            currentAppForm = Qt.createQmlObject(formQML,
                applicationArea, "autoform1");
        }
    }

    onFormComponentChanged: {
        if ( currentAppForm != null){
            currentAppForm.destroy(0)
        }

        if ( formComponent !== null) {
            var component = Qt.createComponent(formComponent)
            currentAppForm = component.createObject(applicationArea, {"x": 0, "y": 0});
        }
    }

    Column {
        anchors.fill: parent
        spacing : 3
        BorderImage {
            id : title
            width: parent.width
            height : formTitle != "" ? 25 : 0
            opacity : formTitle != "" ? 1 : 0
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

    Connections {
        target: currentAppForm
        onFormresultChanged: {
            formResultChanged();
        }
    }
}
