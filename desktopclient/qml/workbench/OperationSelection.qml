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
import "../controls" as Controls

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
        height : 48
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 3
        width : functionBarHeader.width
        x : functionBarHeader.x
        color : Global.alternatecolor1

        Column {
            y : 2
            anchors.fill: parent
            spacing : 3
            Controls.TextEditLabelPair{

                labelText:  qsTr("Filter Text")
                labelWidth: 100
                width : parent.width - 9
                transparentBackgrond: false
                onContentChanged: {
                    operations.nameFilter = content
                }
            }
            Row {
                height : 22
                width : parent.width
                Controls.TextEditLabelPair{
                    id : filtertxt
                    labelText:  qsTr("Keyword filter")
                    labelWidth: 100
                    width : parent.width - 49
                    transparentBackgrond: false
                }
                Button{
                    width : 40
                    height : 20
                    text : qsTr("Apply")
                    onClicked: {
                        operations.keyFilter = filtertxt.content;
                    }
                }
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
