import QtQuick 2.0

import "../Global.js" as Global
import "../controls" as Controls

Item{
    id : currentOperation
    height : 45;
    width : parent.width;

    Controls.ToolTip{
        target : currentOperation
        text : description
    }

    Rectangle {
        anchors.fill: parent
        color: operationsList.currentIndex === index ? Global.selectedColor : index  % 2 == 0 ? uicontext.lightestColor: "white"
        Column {
            id : operationName
            height : parent.height
            width : parent.width - 4
            x : 4
            spacing : 4

            Row {
                width : parent.width - outputIcons.width
                height : 17
                spacing: 3
                Image {
                    y : 7
                    width : parent.height - 5
                    height : parent.height - 5
                    source : specialIcon == "" ? "" : "../images/"  + specialIcon
                    opacity: 0.8
                }

                Text {
                    id : label
                    text : displayName
                    width : parent.width - outputIcons.width - 14
                    height : 17
                    y : 3
                    x : 5
                    font.pointSize: 12
                }
            }
            Text {
                id : pytsyntax
                text : pythonSyntax
                width : parent.width - outputIcons.width
                height : 17
                font.pointSize: 8
                font.italic: true
                opacity : 0.5
                elide: Text.ElideMiddle
            }

        }
        Row {
            anchors.right: parent.right
            anchors.rightMargin: 4
            height : parent.height
            width : childrenRect.width
            spacing : 2
            ListView {
                id : outputIcons
                model : operations.operation(id).outParameterIconList
                //anchors.fill: parent
                height : parent.height
                width : childrenRect.width
                orientation : ListView.Horizontal
                delegate: Component{
                    Image{
                        y : 2
                        width : 21
                        height : 21
                        source : "../images/" + modelData
                    }
                }
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
                operationid = id
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
