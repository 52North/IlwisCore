import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import ".." as Base
import ModellerModel 1.0

Rectangle {
    id : modellerContainer
    property int defaultWidth: defaultFunctionBarWidth
    property var currentAppForm : null
    property var operationid
    property var currentIndex;

    signal unloadcontent(string content)

    color : background4
    clip : true
    width : defaultWidth
    state : "invisible"
    height : 500
    opacity : 1

    signal updateSelectedItem(string name)

    function clear() {
        modelNameText.text = "";
        modelDescriptionText.text = "";

    }

    function addWorkflow(name, description) {
          modellermodel.addWorkflow(name, description)
//        dummyMaps.append( {
//                             name : name,
//                             description: description}
//                         )
        workflowView.currentIndex = workflowView.count-1
    }

    function setWorkflowMetadata(value) {
        if (value !== undefined) {
            modelNameText.text = value.name;
            modelDescriptionText.text = value.description;
            showModellerPane(value.name)
            nameItem.opacity = 1
            descriptionItem.opacity = 1
            createButton.opacity = 0
            deleteButton.opacity = 1
            executeButton.opacity = 1
            showButton.opacity = 1
        }

    }

    function removeWorkflow(index) {
        modellermodel.removeWorkflow(index)
        setWorkflowMetadata(modellermodel.get(index-1))
        //dummyMaps.remove(index)
        //setWorkflowMetadata(dummyMaps.get(index-1))
    }

    function getWorkflow(name) {
        for (var i = 0; i < workflowView.count; i++) {
            if (modellermodel.get(i).name === name) {
                workflowView.currentIndex = i
                return modellermodel.get(i)
            }
        }
//        for (var i = 0; i < workflowView.count; i++) {
//            if (dummyMaps.get(i).name === name) {
//                workflowView.currentIndex = i
//                return dummyMaps.get(i)
//            }
//        }
    }

    function getWorkflowIndex(name) {
        for (var i = 0; i < workflowView.count; i++) {
            if (modellermodel.get(i).name === name) {
                return i
            }
        }
//        for (var i = 0; i < workflowView.count; i++) {
//            if (dummyMaps.get(i).name === name) {
//                return i
//            }
//        }
    }

    onUpdateSelectedItem:  {
        var flow = getWorkflow(name)
        if (flow !== null)
        setWorkflowMetadata(flow)
    }

    FunctionBarHeader{
        id: functionBarHeader
        headerImage: "../images/modellerCS1.png"
        headerText:"Modeller"
    }

    Action {
        id : newModeller
        onTriggered : {
            newButton.enabled = false
            nameItem.opacity = 1
            descriptionItem.opacity = 1
            cancelButton.opacity = 1
            clear();
            deleteButton.opacity = 0
            executeButton.opacity = 0
            showButton.opacity = 0
            createButton.opacity = 0
        }
    }

    Action {
        id : deleteModeller
        onTriggered : {
            newButton.opacity = 1
            var name = modelNameText.text
            removeModellerPane(name)
            clear();
            createButton.opacity = 0
            deleteButton.opacity = 0
            executeButton.opacity = 0
            showButton.opacity = 0
            removeWorkflow(getWorkflowIndex(name))

        }
    }

    Action {
        id : executeModeller
        onTriggered : {
            //console.log("Execute workflow: " + modellermodel.workflows.get(currentIndex).name)
            console.log("Execute workflow: " + dummyMaps.get(currentIndex).name)
        }
    }

    Action {
        id : showModeller
        onTriggered : {
            modellerPane(modelNameText.text)
        }
    }


    Action {
        id : createModeller
        onTriggered : {
            newButton.enabled = true
            cancelButton.opacity = 0
            addWorkflow(modelNameText.text,modelDescriptionText.text)
            modellerPane(modelNameText.text)
            createButton.opacity = 0
            deleteButton.opacity = 1
            executeButton.opacity = 1
            showButton.opacity = 1

        }
    }

    Action {
        id : cancelModeller
        onTriggered : {
            newButton.enabled = true
            nameItem.opacity = 0
            descriptionItem.opacity = 0
            clear();
            cancelButton.opacity = 0
            createButton.opacity = 0
            //setWorkflowMetadata(modellermodel.workflows.get(workflowView.currentIndex))
            setWorkflowMetadata(dummyMaps.get(workflowView.currentIndex))
        }
    }

    ListModel {
        id : dummyMaps
        ListElement {
            name : "GCL_INT__12.mpr"
            description: "raster20.png"
        }
        ListElement {
            name : "woredas.shp"
            description: "feature20.png"
        }
        ListElement {
            name : "average_monthly_temperature_june_7.mpr"
            description: "raster20.png"
        }
    }

    Rectangle {
        id : informationBar
        height : 100
        anchors.top : functionBarHeader.bottom;
        anchors.topMargin: 3
        width : functionBarHeader.width
        x : functionBarHeader.x
        color : background2
        Column {
            id: columns
            anchors.fill: parent
            spacing: 2
            Item{
                id: buttons
                height: 22
                width: parent.width
                Row {
                    spacing: 5
                    Button{
                        id : newButton
                        width : 45
                        height : 20
                        text :  "New"
                        action : newModeller
                    }
                    Button{
                        id : deleteButton
                        width : 45
                        height : 20
                        text :  "Delete"
                        action : deleteModeller
                        opacity: 0
                    }
                    Button{
                        id : executeButton
                        width : 45
                        height : 20
                        text :  "Execute"
                        action : executeModeller
                        opacity: 0
                    }
                    Button{
                        id : showButton
                        width : 45
                        height : 20
                        text :  "Show"
                        action : showModeller
                        opacity: 0
                    }
                }
            }
            Item{
                id: nameItem
                height : 22
                width : parent.width
                opacity: 0
                Text {
                    id : modelNameLabel
                    height : 20
                    text : "Modeller name"
                    width : modelDescriptionLabel.width
                    y : 4
                    x: 5
                }
                TextField {
                    id : modelNameText
                    anchors.left: modelNameLabel.right
                    height : 20
                    width : parent.width - modelNameLabel.width - 9
                    onTextChanged: createButton.opacity = 1
                }
            }
            Item {
                id: descriptionItem
                height : 22
                width : parent.width
                opacity: 0
                Text {
                    id : modelDescriptionLabel
                    x : 5
                    height : 20
                    text : "Model description"
                }
                TextField {
                    id: modelDescriptionText
                    anchors.left: modelDescriptionLabel.right
                    height : 20
                    width : parent.width - modelDescriptionLabel.width  - 9
                }
            }
            Item{
                height : 22
                width : parent.width
                Row {
                    spacing: 5
                    Button{
                        id : createButton
                        width : 45
                        height : 20
                        text :  "Create"
                        action : createModeller
                        opacity: 0
                    }
                    Button{
                        id : cancelButton
                        width : 45
                        height : 20
                        text :  "Cancel"
                        action : cancelModeller
                        opacity: 0
                    }
                }
            }
        }
    }

    Rectangle {
        id : workflowSelectionBar
        height : modellermodel.count() * 20 + headerworkflowSelection.height + 5
        //height : dummyMaps.count * 20 + headerworkflowSelection.height + 5
        anchors.top : informationBar.bottom;
        anchors.topMargin: 3
        width : functionBarHeader.width
        x : functionBarHeader.x
        color : background2
        Column {
            spacing : 6
            width : parent.width
            height : parent.height
            Column {
                id : workflowSelectionColumn
                width : parent.width
                height : modellermodel.count() * 20 + headerworkflowSelection.height
                //height : dummyMaps.count * 20 + headerworkflowSelection.height
                Text {
                    id : headerworkflowSelection
                    text : qsTr("Workflows")
                    font.bold: true
                    font.pointSize: 10
                }
                Rectangle {
                    width : parent.width - 6
                    x : 3
                    y : 3
                    height :workflowSelectionColumn.height - headerworkflowSelection.contentHeight
                    color : background1
                    radius : 10

                    ListView{
                        id : workflowView
                        anchors.fill : parent
                        highlight: Rectangle {
                            width: parent.width
                            height: parent.heightworkflows
                            color: "Green"
                            opacity : 0.2
                            radius: 5
                        }
                        model : modellermodel.workflows
                        //model : dummyMaps
                        focus : true
                        clip : true
                        delegate {
                            Item {
                                width : parent.width
                                height : 18
                                Row {
                                    width : parent.width
                                    height : parent.height
                                    spacing : 3
                                    Text  {
                                        id : dnText
                                        text : modelName
                                        font.pointSize: 9
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                }
                                MouseArea {
                                    hoverEnabled: true
                                    anchors.fill: parent
                                    cursorShape: Qt.ArrowCursor
                                    onClicked: {
                                        workflowView.currentIndex = index
                                        setWorkflowMetadata(modellermodel.get(index))
                                        //setWorkflowMetadata(dummyMaps.get(index))
                                        currentIndex = index
                                    }
                                }
                            }
                        }

                        Component.onCompleted: {
//                            console.log(workflowView.currentIndex);
//                            console.log(modellermodel.get(workflowView.currentIndex).name)
//                            console.log(modellermodel.get(workflowView.currentIndex).modelName)
//                            console.log(modellermodel.count())
                            setWorkflowMetadata(modellermodel.get(workflowView.currentIndex))
                            //setWorkflowMetadata(dummyMaps.get(workflowView.currentIndex))
                        }
                    }
                }
            }
        }
    }


    states: [
        State { name: "visible"

            PropertyChanges {
                target: modellerContainer
                opacity : 1
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: modellerContainer
                opacity : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation {
                properties: "opacity"; duration : 500 ; easing.type: Easing.Linear
            }
            onRunningChanged :
            {
                if ( opacity == 0) {
                    unloadcontent("Modeller.qml")
                }
            }

        }
    ]
}

