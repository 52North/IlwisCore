import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import ".." as Base

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

    FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/operationCS1.png"
        headerText:qsTr("Operations")
    }

    Rectangle {
        id : searchBar
        height : 56
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 3
        width : functionBarHeader.width
        x : functionBarHeader.x
        color : background2
        Column {
            anchors.fill: parent
            Item{
                height : 22
                width : parent.width
                Text {
                    id : searchTextLabel
                    height : 20
                    text : "Filter Text"
                    width : keywordsLabel.width
                    y : 4
                    x: 5
                }
                TextField {
                    id : searchText
                    anchors.left: searchTextLabel.right
                    height : 20
                    width : parent.width - searchTextLabel.width - 9
                }
            }
            Item {
                height : 22
                width : parent.width
                Text {
                    id : keywordsLabel
                    //anchors.top : searchTextLabel.bottom
                    x : 5
                    height : 20
                    text : "Keyword search "
                }
                ComboBox {
                    anchors.left: keywordsLabel.right
                    //anchors.top : searchText.bottom
                    height : 20
                    width : parent.width - searchTextLabel.width  - 9
                    editable: true
                }
            }
        }


    }

    SplitView{
        width : parent.width
        anchors.bottom: container.bottom
        anchors.top : searchBar.bottom;
        anchors.topMargin: 5
        orientation: Qt.Vertical
        Rectangle {
            id : applicationForm
            width : parent.width
            height : 0
            x : parent.x + 5

            ApplicationForm{
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
                OperationList{
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
                OperationCatagoriesList{
                    id : operationCatagories
                    Connections {
                        target : operationCatagories
                        onMakeForm : {
                            console.debug("name :", name)
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
                    unloadcontent("OperationSelection.qml")
                }
            }
        }


    ]

}
