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

Rectangle {
    id : container

    property var currentAppForm : null
    property var operationid

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

    Workbench.FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/modellerCS1.png"
        headerText:qsTr("Modeller")
    }

    Action {
        id : newModeller
        onTriggered : {
            newButton.enabled = false
             // create Modeller obeject
        }
    }

    Action {
        id : deleteModeller
        onTriggered : {
            operationTabs.activ
            newButton.opacity = 1
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
                        text :  "New"
                        action : newModeller
                        enabled: true
                    }
                    Button{
                        id : deleteButton
                        width : 45
                        height : 20
                        text :  "Delete"
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
            Button{
                y : parent.height - 25
                width : 50
                text : "execute"
                height : 22
                x : parent.width - 60
                onClicked: appFrame.doExecute(operationid)
            }
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
                            console.log(objectid)
                             console.log(name)
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
