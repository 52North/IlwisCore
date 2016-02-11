import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../Global.js" as Global

Rectangle {
    id : serviceConnectionArea

    property string formQML : ""
    property var currentAppForm : null
   // property string exprparameters
    property var operationid

    onFormQMLChanged: {
        if ( currentAppForm != null){
            currentAppForm.destroy(0)
        }

        if ( formQML.length !== 0) {
            currentAppForm = Qt.createQmlObject(formQML,
                formFrame, "autoform2");
        }
    }

    anchors.fill: parent
    color : "white"
    Item {
        id : protocol
        width : parent.width
        height : 22
        anchors.top: parent.top
        anchors.topMargin: 5
        Text {
            id : sclabel2
            text : "protocols"
            width : 100
            height : 20
            x : 5
        }
        ComboBox{
            id : cbprotocols
            model : operations.serviceNames()
            width : parent.width - sclabel2.width - 7
            height : 20
            anchors.left : sclabel2.right
            onCurrentIndexChanged: {
                operationid = operations.serviceId(currentIndex)
                formQML = formbuilder.index2Form(operationid, true)
            }
        }
    }
    Item {
        id : formFrame

        anchors.top: protocol.bottom
        anchors.topMargin: 10
        width : parent.width
        height : 250

    }

    Action {
        id : makeUrl
        onTriggered: {
            if (currentAppForm != null){
                var parms = currentAppForm.formresult + "connectionstring"
                currentAppForm.outputfield_0 = operations.executeoperation(operationid, currentAppForm.formresult)
            }
        }
    }

    Action {
        id : makeConnection
        onTriggered: {
            if (currentAppForm != null){
                //mastercatalog.addCatalog(currentAppForm.outputfield_0)
                var parts = currentAppForm.formresult.split("|")
                var url = parts[parts.length - 1]
                url.split('\"').join('')
                var cmd = "container='" + url +"'"
                bigthing.newCatalog(cmd,"catalog",url, "right")
                currentFolder = url
            }
        }
    }


    Item {
        //y : currentAppForm != null ? currentAppForm.height  + protocol.height + 60: formFrame.height + protocol.height + 40
        anchors.bottom: serviceConnectionArea.bottom
        anchors.bottomMargin: 12
        anchors.right: parent.right
        anchors.rightMargin: 35
        width : 100
        height : 15
        Row {
            width : parent.width
            height : parent.height
            spacing : 5
            Button{
                text : "Make Url"
                action : makeUrl
            }
            Button {
                action : makeConnection
                text : "Connect"
            }
        }
    }
}
