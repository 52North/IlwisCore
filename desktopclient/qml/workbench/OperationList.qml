import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.1
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0

import "../Global.js" as Global

Rectangle {

    signal makeForm(string objectid, string name, string url)

    property var operationsModel : operations.operations
    property bool byKey : false


    width : parent.width
    color : "white"
    opacity: width > 4 ? 1 : 0
    clip : true


    ListView {
        id : operationsList
        anchors.fill: parent
        model : operationsModel
        interactive : !byKey
        delegate : Item{
            id : currentOperation
            height : 35;
            width : parent.width;
            Rectangle {
                anchors.fill: parent
                color: operationsList.currentIndex === index ? Global.selectedColor : index  % 2 == 0 ? "#F7F9FC" : "#DCDCDC"
                Text {
                    id : operationName
                    text : displayName
                    anchors.top : parent.top
                    width : parent.width
                    height : 17
                    x : 5
                    font.pointSize: 12
                }
                Text {
                    id : operationSyntax
                    text : syntax
                    anchors.top : operationName.bottom
                    width : parent.width
                    height : 12
                    font.pointSize: 8
                    x : 5
                    color : "grey"
                }
            }

            focus : true
            MouseArea {
                id: mouseArea
                hoverEnabled: true
                anchors.fill: parent
                cursorShape: Qt.ArrowCursor


                onClicked: {
                    applicationForm.state = operationsList.currentIndex == index && applicationForm.state != "minimized" ? "minimized" : "maximized"
                    operationsList.currentIndex = index;
                    makeForm(id, displayName, url)
                }
                onPressed:{
                    drag.target =  Qt.createQmlObject('import QtQuick 2.0; Image{
                    id : image
                    width : 20; height : 20
                    source : "../images/operation20.png"
                    fillMode: Image.PreserveAspectFit
                    x : mouseArea.mouseX + 50
                    property string message :  model !== null ? url : ""
                    property string ilwisobjectid : model !== null ? id : ""
                    property string type : model !== null ? typeName : ""
                    property string ids : model !== null ? mastercatalog.selectedIds() : ""

                    Drag.keys: typeName
                    Drag.active: mouseArea.drag.active
                    Drag.hotSpot.x: 10
                    Drag.hotSpot.y: 10
                    opacity : Drag.active / 2

                    states: State {
                        when: mouseArea.drag.active
                        ParentChange { target: image; parent: root }
                        AnchorChanges { target: image; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
                    }
                }', mouseArea, "dynamicImage");
                }
                onReleased: {
                    drag.target.Drag.drop()
                    drag.target.parent = mouseArea
                    drag.target.anchors.fill = mouseArea
                    drag.target.destroy();
                }
            }

        }
    }

}
