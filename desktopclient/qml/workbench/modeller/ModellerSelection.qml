import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../.." as Base
import "../../workbench" as Workbench
import "../../datapanel" as DataPane

Rectangle {
    id : container

    property string newText : qsTr("New");
    property string createText : qsTr("Create");

    property var currentAppForm : null
    property var operationid

    property int modellerCount : 0;

    color : background4
    clip : true
    state : "invisible"
    opacity : 0

    signal unloadcontent(string content)

    function newForm(metaid, title){
        var form= formbuilder.index2Form(metaid)
        appFrame.formQML = form
        appFrame.formTitle = title
        appFrame.opacity = 1
    }

    function newFormUrl(url, title){
        appFrame.formComponent = url
        appFrame.formTitle = title
        appFrame.opacity = 1
    }

    Workbench.FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/modellerCS1.png"
        headerText:qsTr("Modeller")
    }

    Action {
        id : newModeller
        onTriggered : {
            if (newButton.text == newText) {
                newButton.text = createText
                cancelButton.opacity = 1
                applicationForm.state = "maximized";
                newFormUrl("modeller/ModellerMetadataForm.qml", "New Modeller");
                // create temporary Modeller obeject
                // open formular
            } else if (newButton.text == createText) {
                var modeller = "Wokflow-";
                if (appFrame.currentAppForm != null && appFrame.currentAppForm.getName() !== null && appFrame.currentAppForm.getName().length > 0) {
                    modeller += appFrame.currentAppForm.getName();
                    newButton.text = newText
                    cancelButton.opacity = 0
                    // create Modeller obeject from temporary object
                    // open modeller panel
                    dataPanel.addModellerPanel(modeller);
                    applicationForm.state = "minimized";
                } else {
                    var message = Qt.createQmlObject('import QtQuick 2.2; import QtQuick.Dialogs 1.1; MessageDialog {
                         id: messageDialog
                         title: "Error!";
                         text: "Missing workflow name! Please enter a workflow name!";
                         icon: StandardIcon.Warning;
                         modality: Qt.ApplicationModal
                         onAccepted: {
                            messageDialog.close();
                            messageDialog.destroy();
                         }
                         Component.onCompleted: visible = true
                     }', newModeller, "dynamicMessage");
                }
            }
        }
    }

    Action {
        id: cancelModeller
         onTriggered : {
             cancelButton.opacity = 0
             newButton.text = newText
             applicationForm.state = "minimized";
             // delete temporary Modeller obeject
         }
    }

    Action {
        id : deleteModeller
        onTriggered : {
            operationTabs.activ
        }
    }

    Rectangle {
        id : searchBar
        height : 24
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 3
        width : functionBarHeader.width
        x : functionBarHeader.x
        color : background2

        Text {
            id : searchTextLabel
            height : 20
            text : qsTr("Filter Text")
            width : 100
            y : 4
            x: 5
        }
        TextField {
            id : searchText
            anchors.left: searchTextLabel.right
            height : 20
            width : parent.width - searchTextLabel.width - 9
            onTextChanged: {
                operations.nameFilter = text
            }
        }
    }
    Rectangle {
        id : modellerBar
        height : 24
        anchors.top : searchBar.bottom;
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
                        text :  newText
                        action : newModeller
                        enabled: true
                    }
                    Button{
                        id : cancelButton
                        width : 45
                        height : 20
                        text :  qsTr("Cancel")
                        action : cancelModeller
                        enabled: true
                        opacity: 0
                    }
                    Button{
                        id : deleteButton
                        width : 45
                        height : 20
                        text :  qsTr("Delete")
                        action : deleteModeller
                        opacity: 0
                    }
                }
            }
        }
    }
    SplitView{
        width : parent.width
        anchors.bottom: container.bottom
        anchors.top : modellerBar.bottom;
        anchors.topMargin: 5
        orientation: Qt.Vertical
        Rectangle {
            id : applicationForm
            width : parent.width
            height : 0
            x : parent.x + 5

            Workbench.ApplicationForm{
                id : appFrame
                width : parent.width
                height : parent.height - 30 < 0 ?  0 : parent.height - 30
                opacity : 0

            }
            //            Button{
            //                y : parent.height - 25
            //                width : 50
            //                text : "execute"
            //                height : 22
            //                x : parent.width - 60
            //                onClicked: appFrame.doExecute(operationid)
            //            }
            states: [
                State { name: "maximized"

                    PropertyChanges {
                        target: applicationForm
                        height : 300
                        opacity : 1

                    }
                },
                State {
                    name : "minimized"
                    PropertyChanges {
                        target: applicationForm
                        height : 0
                        opacity : 0
                    }
                }

            ]
            transitions: [
                Transition {
                    NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
                    NumberAnimation { properties: "opacity"; duration : 750 ; easing.type: Easing.InOutCubic }
                }
            ]
        }
        TabView{
            id : operationTabs
            width : parent.width
            Tab {
                title : qsTr("Operation List")
                signal makeForm(string objectid, string name)
                ModellerList{
                    Connections {
                        target : operationList
                        onMakeForm : {
                            newForm(objectid, name)
                        }
                    }
                    id : operationList
                }


            }
            Tab {
                title : qsTr("Operation Categories")
                 signal makeForm(string objectid, string name)
                ModellerCatagoriesList{
                    id : operationCatagories
                    Connections {
                        target : operationCatagories
                        onMakeForm : {
                            newForm(objectid, name)
                        }
                    }
                }

            }
            style: Base.TabStyle1{}
        }
    }

    states: [
        // This adds a second state to the container where the rectangle is farther to the right

        State { name: "visible"

            PropertyChanges {
                target: container
                opacity : 1
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: container
                opacity : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "opacity"; duration : 500 ; easing.type: Easing.Linear }
            onRunningChanged :
            {
                if ( opacity == 0) {
                    unloadcontent("ModellerSelection.qml")
                }
            }
        }


    ]

}
