import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import ".." as Base
import "../Global.js" as Global

WorkBenchShifter {
    id : operationSelectionContainer

    property var currentAppForm : null
    property var operationid

    clip : true
    state : "invisible"

    function newForm(metaid, title, url){
        operationid = metaid
        var form= formbuilder.index2Form(metaid, true)
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
        height : 24
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 3
        width : functionBarHeader.width
        x : functionBarHeader.x
        color : Global.alternatecolor1

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
    SplitView{
        width : parent.width
        anchors.bottom: operationSelectionContainer.bottom
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
                        height : appFrame ? appFrame.height : 350
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
        TabView {

            id : operationTabs
            width : parent.width

            Tab {
                title : qsTr("Operation List")
                OperationList {
                    id : operationList
                    Connections {
                        target : operationList
                        onMakeForm : {
                            newForm(objectid, name)
                        }
                    }
                }


            }
            Tab {
                title : qsTr("Operation Categories")
                OperationCatagoriesList{
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

}
