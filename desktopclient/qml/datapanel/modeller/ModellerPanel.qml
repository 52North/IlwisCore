import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import "../catalog" as Parent

Item {
    property int heightButtons : 26
    id : modellerViews
    width : parent.width
    height : parent.height

    signal modellerChanged()

    ToolBar{
        id : toolbar
        anchors.top : parent.top
        width : parent.width
        //height : 35

        Action {
            id :refreshCatalog
            onTriggered: {
                //canvas.clear();
            }
        }

        Action {
            id :zoomInAction
            onTriggered: {
                //canvas.zoomIn();
            }
        }

        Action {
            id :zoomOutAction
            onTriggered: {
               // canvas.zoomOut();
            }
        }

        RowLayout{
            spacing: 1

            ToolButton{
                id : zoomIn
                height: toolbar.height
                //iconSource: "../../images/plus.png"
                Image {
                    source: "../../images/plus.png"
                    anchors.fill: parent
                    anchors.margins: 4
                }
                checkable: true
                action: zoomInAction
            }
            ToolButton{
                id : zoomOut
                height: toolbar.height
                //iconSource: "../../images/minus.png"
                Image {
                    source: "../../images/minus.png"
                    anchors.fill: parent
                    anchors.margins: 4
                }
                checkable: true
                action: zoomOutAction
            }

            ToolButton{
                id : refresh
                implicitHeight: heightButtons
                iconSource: "../../images/refresh20.png"
                tooltip: "Refresh"
                action : refreshCatalog
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

            DataSource {
                id: ds
            }

            Operation {
                id: ops

            }

        }
    }

}
