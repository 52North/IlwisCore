import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0

Item {
    property int heightButtons : 26
    id : modellerViews
    width : parent.width
    height : parent.height
    property bool canSeparate : false

    ToolBar{
        id : toolbar
        anchors.top : parent.top
        width : parent.width
        //height : 35

        Action {
            id :clear
            onTriggered: {
                drawing.clearModeller();
            }
        }

        Action {
            id :newDatasource
            onTriggered: {
                drawing.addType(Qt.createComponent("DataSourceObject.qml").createObject())
            }
        }

        Action {
            id :newOperation
            onTriggered: {
                 drawing.addType(Qt.createComponent("OperationObject.qml").createObject())
            }
        }


        Action {
            id :newConnection
            onTriggered: {
                 drawing.addType(Qt.createComponent("ConnectorObject.qml").createObject())
            }
        }

        Action {
            id :save
            onTriggered: {
                console.log("Currently not supported!")
            }
        }

        Action {
            id :close
            onTriggered: {
                drawing.clearModeller();
                dataPanel.removeModellerPanel(parent.title);
            }
        }
        RowLayout{
            spacing: 1

            ToolButton{
                id : newDatasourceButton
                height: toolbar.height
//                //iconSource: "../../images/plus.png"
//                Image {
//                    source: "../../images/plus.png"
//                    anchors.fill: parent
//                    anchors.margins: 4
//                }
                text: qsTr("Datasource")
                checkable: true
                action: newDatasource
            }
            ToolButton{
                id : newOperationButton
                height: toolbar.height
//                //iconSource: "../../images/minus.png"
//                Image {
//                    source: "../../images/minus.png"
//                    anchors.fill: parent
//                    anchors.margins: 4
//                }
                text: qsTr("Operation")
                checkable: true
                action: newOperation
            }
            ToolButton{
                id : newConnectionButton
                height: toolbar.height
//                //iconSource: "../../images/minus.png"
//                Image {
//                    source: "../../images/minus.png"
//                    anchors.fill: parent
//                    anchors.margins: 4
//                }
                text: qsTr("Connection")
                checkable: true
                action: newConnection
            }
            ToolButton{
                id : clearButton
                height:  toolbar.height
                text: qsTr("Clear")
                action : clear
            }

            ToolButton{
                id : saveButton
                height:  toolbar.height
                text: qsTr("Save")
                action : save
            }

            Rectangle{
                implicitHeight: heightButtons
                width: 20
                color : "grey"
                opacity: 0
            }

            ToolButton{
                id : closeButton
                height:  toolbar.height
                text: qsTr("Close")
                action : close
            }

        }

    }

    Rectangle {
        width : parent.width
        anchors.top: toolbar.bottom
        anchors.bottom: parent.bottom
        color: "white"
        property int paintX
        property int paintY

        Drawing {
            id: drawing
            anchors.fill: parent

        }
    }

}
