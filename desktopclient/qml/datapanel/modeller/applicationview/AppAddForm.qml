import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global

Item {
    id : createForm
    state : "invisible"
    height : parent.height
    property string appType

    property string selectedApp

    onAppTypeChanged: {
        names.model = null
        var apps = modellerDataPane.model.applicationsByAnalysis(appType)
        if ( apps.length > 0)
            selectedApp = apps[0]
        names.model = apps
    }


    Column {
        width : parent.width - 3
        height : parent.height
        spacing : 3
        x : 3
        Rectangle {
            height : parent.height - 30
            width : 300
            border.width: 1
            border.color: Global.edgecolor

            ListView{
                id : names
                anchors.fill: parent
                highlight : Rectangle{width : parent.width;height : 18;color : Global.selectedColor}
                model : modellerDataPane.model ? modellerDataPane.model.applicationsByAnalysis(appType) : null
                delegate: Text{
                    height : 16
                    width : 200
                    text : modelData
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            names.currentIndex = index
                            selectedApp = modelData
                        }
                    }
                }

            }
        }

        Row {
            width : 200
            height : 22
            spacing : 4
            Button {
                id : applyBut
                width : 100
                height : 22
                text : qsTr("Add")
                onClicked: {
                    var parms = {modelId : modellerDataPane.model.id, subtype : selectedApp, type : "applicationmodel"}
                    var app = objectcreator.createModellerObject(parms,0)
                    if (!modellerDataPane.model.addApplication(app)){
                        app.suicide()
                    }else {
                        layerprops.refreshAppNames()
                        applicationView.currentApplication = app
                        appv.refreshFormArea(app.panel("form"))
                    }
                }
            }
            Button {
                width : 100
                height : 22
                text : qsTr("Close")
                onClicked: {
                    createForm.state = "invisible"
                }
            }
        }
    }
     states: [
        State { name: "invisible"

            PropertyChanges {
                target: createForm
                width : 0
            }
            PropertyChanges {
                target: rest
                opacity : 1
            }
        },
        State {
            name : "visible"
            PropertyChanges {
                target: createForm
                width : layerprops.width - firstColumn.width - 3
            }
            PropertyChanges {
                target: rest
                opacity : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "width"; duration : 630 ; easing.type: Easing.InOutCubic }
            onRunningChanged: {
                if ((state == "invisible") && (!running)) {
                    opacity = 0
                }else
                    opacity = 1
            }
        }
    ]
}
