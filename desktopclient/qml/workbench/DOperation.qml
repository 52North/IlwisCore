import QtQuick 2.0

import "../Global.js" as Global

Item{
    id : currentOperation
    height : 35;
    width : parent.width;
    Rectangle {
        anchors.fill: parent
        color: operationsList.currentIndex === index ? Global.selectedColor : index  % 2 == 0 ? Global.alternatecolor4 : Global.alternatecolor2
        Row {
            id : operationName
            height : 17
            width : parent.width - 4
            x : 4
            spacing : 4

            Text {
                id : label
                text : displayName
                anchors.top : parent.top
                width : parent.width - outputIcons.width - inputIcons.width
                height : 17
                x : 5
                font.pointSize: 12
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
                        width : 10
                        height : 10
                        source : "../images/" + modelData
                    }
                }
            }
            Text {
                width : 7
                height : parent.height
                text : "="
            }
            Text {
                width : 7
                height : parent.height
                text : "("
            }

            ListView {
                id : inputIcons
                model : operations.operation(id).inParameterIconList
                //anchors.fill: parent
                height : parent.height
                width : childrenRect.width

                orientation : ListView.Horizontal
                visible : currentOperation.width > 200
                delegate: Component{
                    Image{
                        y : 2
                        x : 3
                        width : 10
                        height : 10
                        source : "../images/" + modelData
                    }
                }
            }
            Text {
                width : 7
                height : parent.height
                text : ")"
            }
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

