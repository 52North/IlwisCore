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
    property string analysisType


    Column {
        width : parent.width - 3
        height : parent.height
        spacing : 3
        x : 3
        Controls.TextEditLabelPair{
            id : namefield
            width : parent.width
            labelWidth: 80
            labelText: qsTr("Name")
            transparentBackgrond: false
        }
        Controls.TextEditLabelPair{
            id : longnamefield
            width : parent.width
            labelWidth: 80
            labelText: qsTr("Long Name")
            transparentBackgrond: false
        }
        Controls.TextEditLabelPair{
            width : parent.width
            labelWidth: 80
            labelText: qsTr("Owner")
            transparentBackgrond: false
        }
        Controls.TextEditLabelPair{
            width : 200
            labelWidth: 80
            labelText: qsTr("Time")
            readOnly: true

        }
        Controls.SpatialSelectionDropDown{
            width : parent.width
            height : 22
            labelText: qsTr("Study B. Box")
        }

        Controls.TextAreaLabelPair{
            id : descField
            width : parent.width
            labelWidth: 80
            height : parent.height - 160
            labelText: qsTr("Description")
        }
        Row {
            width : 200
            height : 22
            spacing : 4
            Button {
                id : applyBut
                width : 100
                height : 22
                text : qsTr("Create")
                onClicked: {
                    var parms = {modelId : modellerDataPane.model.id, subtype : analysisType, type : "analysispattern",pattername : namefield.content, description : descField.content}
                    var analysis = objectcreator.createModellerObject(parms,0)
                    if (!modellerDataPane.model.addAnalysisPattern(analysis)){
                        if ( analysis)
                            analysis.suicide()
                    }else {
                        analysis.longName = longnamefield.content
                        layerprops.refreshAnalysisNames()
                        layerprops.selectLastListItem()
                        createForm.state = "invisible"
                        analisysView.currentAnalysis = analysis
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
            NumberAnimation { properties: "opacity"; duration : 630 ; easing.type: Easing.InOutCubic }
            onRunningChanged: {
                if ((state == "invisible") && (!running)) {
                    opacity = 0
                }else{
                    opacity = 1
                }

            }
        }
    ]
}
