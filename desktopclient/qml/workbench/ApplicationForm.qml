import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick 2.0
import "../Global.js" as Global

Rectangle {
    property color background1 : "white"
    property string formQML : ""
    property var formComponent : null
    property var currentAppForm : null
    property string formTitle
    property string operationId : "id"
    property string operationUrl : "url"
    property bool showTitle: true

    signal formResultChanged();

    id : applicationFormFrame
    width : parent.width
    height : parent.height
    radius : 10
    color : background1

    function doExecute(operationid, runparms) {
        if ( currentAppForm != null) {
            operations.executeoperation(operationid,currentAppForm.formresult, runparms)
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
           height = currentAppForm.childrenRect.height + 60
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
        Rectangle {
            id : title
            width: parent.width
            height : formTitle != "" && showTitle ? 25 : 0
            opacity : formTitle != "" && showTitle ? 1 : 0
            color : uicontext.paleColor
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
