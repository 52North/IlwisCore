import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.1
import SMCE 1.0

import "../../../../qml/Global.js" as Global

Rectangle {
    id: smceTree
    anchors.fill: parent
    signal selNodeChanged(var node)
    signal selItemChanged(var item)
    property var selectedItem: null
    property var tree : smcePanel.manager.analisysView.currentAnalysis.tree()
    property var inPlaceEdit: null
    property var clickTimer: null

    function selectItem(item) {
        if (selectedItem != null) {
            selectedItem.state = "unselected"
            selectedItem.focus = false
        }
        if (item != null) {
            item.state = "selected"
            item.forceActiveFocus()
            selectedItem = item
        }
        selItemChanged(item)
    }

    function saveEditAndFinish() {
        cancelTimer()
        if ( inPlaceEdit != null) {
            inPlaceEdit.accepted()
            inPlaceEdit.destroy(0)
            inPlaceEdit = null;
        }
    }

    function cancelTimer() {
        if (clickTimer != null) {
            clickTimer.destroy(0)
            clickTimer = null
        }
    }

    MouseArea {
        anchors.fill: parent
        onPressed: {
            saveEditAndFinish()
            selectItem(null)
        }
    }
    Column {
        anchors.fill: parent
        TableView {
            id: header
            height: 17
            width: scroll.flickableItem.contentX + Math.max(parent.width, criteriaTree.width + col1.width + 2)
            x: -scroll.flickableItem.contentX
            TableViewColumn {
                id: criteriaTree
                title: "Criteria Tree"
                movable: false
                width: 250
            }
            TableViewColumn {
                id: col1
                title: ""
                movable: false
                width: 250
            }
        }
        ScrollView {
            id: scroll
            width: parent.width
            height: parent.height - header.height
            Column {
                ListView {
                    id: objView
                    model: tree
                    width: criteriaTree.width + col1.width
                    height: childrenRect.height
                    delegate: objRecursiveDelegate
                }
                Component {
                    id: objRecursiveDelegate

                    Column {
                        id: objRecursiveColumn
                        property bool treeChild: true
                        spacing: 1
                        state: "expanded"

                        Connections {
                            target: model.modelData
                            onNodeDeleted: {
                                if (selectedItem != null) {
                                    var par = selectedItem.parent.parent.parent.parent.parent
                                    var parMouseArea = par.children[0].children[0].children[0]
                                    parMouseArea.onPressed(null)
                                }
                            }
                        }

                        function toggleNode() {
                            var enabled
                            if (state == "expanded") {
                                state = "collapsed"
                                enabled = false
                            } else {
                                state = "expanded"
                                enabled = true
                            }
                            for(var i = 0; i < children.length; ++i) {
                                if(children[i].treeChild) {
                                    children[i].visible = enabled
                                }
                            }
                        }

                        function getTreeChildren() {
                            var list = []
                            for(var i = 0; i < children.length; ++i) {
                                if(children[i].treeChild) {
                                    list.push(children[i])
                                }
                            }
                            return list
                        }

                        function hasChild() {
                            if (state == "expanded")
                                return getTreeChildren().length > 0
                            else
                                return false
                        }

                        function firstChild() {
                            return getTreeChildren()[0]
                        }

                        function lastChild() {
                            var list = getTreeChildren()
                            return list[list.length - 1]
                        }

                        function prevSibling() {
                            if (parent.treeChild) {
                                var list = parent.getTreeChildren()
                                var ind = list.indexOf(objRecursiveColumn)
                                if (ind > 0)
                                    return list[ind-1]
                                else
                                    return null
                            } else
                                return null
                        }

                        function nextSibling() {
                            if (parent.treeChild) {
                                var list = parent.getTreeChildren()
                                var ind = list.indexOf(objRecursiveColumn)
                                if (ind < list.length - 1)
                                    return list[ind+1]
                                else
                                    return null
                            } else
                                return null
                        }

                        function selectUp(i) {
                            var item = prevSibling()
                            if (item) {
                                while (item.hasChild())
                                    item = item.lastChild()
                            } else if (parent.treeChild)
                                item = parent
                            if (item)
                                item.col(i)
                        }

                        function selectDown(i) {
                            var item = null
                            if (hasChild())
                                item = firstChild()
                            else {
                                var self = objRecursiveColumn
                                item = self.nextSibling()
                                while (!item && self && self.parent.treeChild) {
                                    self = self.parent
                                    item = self.nextSibling()
                                }
                            }
                            if (item)
                                item.col(i)
                        }

                        function col(i) {
                            switch(i) {
                            case 0:
                                selectItem(objTextRowRect)
                                selNodeChanged(model)
                                break;
                            case 1:
                                selectItem(col1Rect)
                                selNodeChanged(null)
                                break;
                            }
                        }

                        Row {
                            Rectangle {
                                width: criteriaTree.width
                                height: childrenRect.height
                                MouseArea {
                                    id: objMouseArea
                                    anchors.fill: parent
                                    onDoubleClicked: {
                                        if (model.type === Node.Group) {
                                            toggleNode()
                                        }
                                    }
                                    onPressed: {
                                        saveEditAndFinish()
                                        col(0)
                                    }
                                }

                                Row {
                                    function getIcon(nodetype, level) {
                                        if (nodetype === Node.Group)
                                            if (level === 0)
                                                return "SMCETree.BMP"
                                            else
                                                return "SMCEGroup.BMP"
                                        else if (nodetype === Node.Constraint)
                                            return "SMCEMapConstraint.bmp"
                                        else if (nodetype === Node.Factor)
                                            return "SMCEMapEffect.bmp"
                                        else if (nodetype === Node.MaskArea)
                                            return "raster.png"
                                    }
                                    Item {
                                        id: objIndentation
                                        height: 20
                                        width: model.level * 30
                                    }
                                    Image {
                                        id: subArrow
                                        width:  15
                                        height:  15
                                        source: "arrowrightlight.png"
                                        rotation: 90
                                        visible: objRepeater.count > 0
                                        MouseArea {
                                            id: objMouseAreaArrow
                                            width: subArrow.implicitWidth
                                            height: subArrow.implicitHeight
                                            onPressed: {
                                                saveEditAndFinish()
                                                col(0)
                                                toggleNode()
                                            }
                                        }
                                    }

                                    Image {
                                        id: icon
                                        source: parent.getIcon(model.type, model.level)
                                        fillMode: Image.Pad
                                    }

                                    Rectangle {
                                        id: objTextRowRect
                                        width: childrenRect.width
                                        height: parent.height
                                        state: "unselected"

                                        function selectLastChild() {
                                            lastChild().col(0)
                                        }

                                        Keys.onPressed: {
                                            switch (event.key) {
                                            case Qt.Key_Delete:
                                                if (model.modelData.smceMode() === Node.EditTree)
                                                    model.modelData.deleteNode()
                                                break
                                            case Qt.Key_Left:
                                                if(model.type === Node.Group && objRecursiveColumn.state == "expanded")
                                                    objRecursiveColumn.toggleNode()
                                                else if (objRecursiveColumn.parent.treeChild)
                                                    objRecursiveColumn.parent.col(0)
                                                break
                                            case Qt.Key_Right:
                                                if(model.type === Node.Group && objRecursiveColumn.state == "collapsed")
                                                    objRecursiveColumn.toggleNode()
                                                else
                                                    col(1)
                                                break
                                            case Qt.Key_Up:
                                                selectUp(0)
                                                break
                                            case Qt.Key_Down:
                                                selectDown(0)
                                                break
                                            }
                                        }

                                        Row {
                                            Text {
                                                id: objNodeWeight
                                                text: "  " + ((model.weight >= 0) ? (model.weight.toFixed(2).toString() + "  ") : "")
                                                color: "black"
                                                verticalAlignment: Text.AlignVCenter
                                            }
                                            Text {
                                                id: objNodeName
                                                text: model.name
                                                color: "black"
                                                verticalAlignment: Text.AlignVCenter
                                                width: criteriaTree.width - objIndentation.width - (subArrow.visible ? subArrow.width : 0) - icon.width - objNodeWeight.width
                                                elide: Text.ElideRight
                                            }
                                        }

                                        states: [
                                            State {
                                                name: "unselected"
                                                PropertyChanges {
                                                    target: objTextRowRect
                                                    color: model.nodeDone ? "white" : "#FFD3D3"
                                                }
                                            },
                                            State {
                                                name: "selected"
                                                PropertyChanges {
                                                    target: objTextRowRect
                                                    color: Global.selectedColor
                                                }
                                            }
                                        ]
                                    }
                                }
                            }
                            Rectangle {
                                id: col1Rect
                                width: col1.width
                                height: childrenRect.height
                                state: "unselected"

                                function markDropCandidate(selected) {
                                    border.color = selected ? "gray" : "transparent"
                                }

                                Keys.onPressed: {
                                    switch (event.key) {
                                    case Qt.Key_F2:
                                    case Qt.Key_Enter:
                                    case Qt.Key_Return:
                                        if (model.modelData.smceMode() === Node.EditTree)
                                            col1MouseArea.startEdit()
                                        break
                                    case Qt.Key_Delete:
                                        if (model.modelData.smceMode() === Node.EditTree)
                                            model.fileName = ""
                                        break
                                    case Qt.Key_Left:
                                        col(0)
                                        break
                                    case Qt.Key_Up:
                                        selectUp(1)
                                        break
                                    case Qt.Key_Down:
                                        selectDown(1)
                                        break
                                    }
                                }

                                MouseArea {
                                    id: col1MouseArea
                                    anchors.fill: parent
                                    property bool selectionChanged: false
                                    onDoubleClicked: {
                                        if (model.fileName !== "") {
                                            cancelTimer()
                                            console.log("openMap("+model.fileName+")")
                                            openMap(model.fileName)
                                        } else if (model.modelData.smceMode() === Node.EditTree)
                                            startEdit()
                                    }

                                    onPressed: {
                                        saveEditAndFinish()
                                        if (selectedItem != col1Rect) {
                                            col(1)
                                            selectionChanged = true
                                        }
                                    }

                                    onClicked: {
                                        if (selectionChanged) {
                                            selectionChanged = false
                                        } else if (model.modelData.smceMode() === Node.EditTree) {
                                            if (clickTimer != null) {
                                                startEdit()
                                            } else
                                                clickTimer = Qt.createQmlObject("import QtQuick 2.0; Timer { id: clickTimer; interval: 500; running: true; onTriggered: {col1MouseArea.startEdit()}}", col1Rect, "clickTimer")
                                        }
                                    }

                                    function startEdit() {
                                        cancelTimer()
                                        if (model.type === Node.Group)
                                            inPlaceEdit = Qt.createQmlObject("import QtQuick 2.0; import QtQuick.Controls 1.0; TextField { id: inPlaceEdit; width: parent.width; height: parent.height; text: model.fileName; verticalAlignment: TextInput.AlignVCenter; Keys.onPressed: {event.accepted=true} Keys.onEscapePressed: {while(canUndo) undo(); editingFinished()} onAccepted: {model.fileName = text; editingFinished()} onEditingFinished: {focus = false; visible = false; col1Rect.forceActiveFocus()} Component.onCompleted: {forceActiveFocus(); selectAll()}}", col1Rect, "inPlaceEdit");
                                        else
                                            flash.running = true
                                    }
                                }
                                DropArea {
                                    anchors.fill: parent
                                    function dropAllowed(event) {
                                        return (model.type !== Node.Group) && (event.keys == "rastercoverage")
                                    }

                                    onDropped: {
                                        if (dropAllowed(drop) && (model.modelData.smceMode() === Node.EditTree)) {
                                            model.fileName = drop.source.message
                                        }
                                        col1MouseArea.cursorShape = Qt.ArrowCursor
                                        col1Rect.markDropCandidate(false)
                                    }
                                    onEntered: {
                                        if (model.modelData.smceMode() === Node.EditTree) {
                                            if (dropAllowed(drag)) {
                                                col1Rect.markDropCandidate(true)
                                            } else {
                                                col1MouseArea.cursorShape = Qt.ForbiddenCursor
                                            }
                                        }
                                    }
                                    onExited: {
                                        col1MouseArea.cursorShape = Qt.ArrowCursor
                                        col1Rect.markDropCandidate(false)
                                    }
                                }

                                Row {
                                    Image {
                                        id: col1IconRaster
                                        source: "raster.png"
                                        fillMode: Image.Pad
                                        opacity: model.fileName !== "" ? 1.0 : 0.0
                                    }

                                    Text {
                                        id: col1NodeName
                                        text: "  " + model.fileName
                                        color: "black"
                                        verticalAlignment: Text.AlignVCenter
                                        width: col1.width - col1IconRaster.width - col1NodeUnit.width
                                        elide: Text.ElideRight
                                    }

                                    Text {
                                        id: col1NodeUnit
                                        visible: model.unit !== ""
                                        text: (model.unit !== "") ? ("[" + model.unit + "]") : ""
                                        color: "black"
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                }

                                SequentialAnimation {
                                    id: flash
                                    PropertyAnimation {
                                        target: col1Rect
                                        property: "color"
                                        to: "white"
                                        duration: 0
                                    }
                                    PropertyAnimation {
                                        target: col1Rect
                                        property: "color"
                                        to: "white"
                                        duration: 75
                                    }
                                    PropertyAnimation {
                                        target: col1Rect
                                        property: "color"
                                        to: Global.selectedColor
                                        duration: 0
                                    }
                                    PropertyAnimation {
                                        target: col1Rect
                                        property: "color"
                                        to: Global.selectedColor
                                        duration: 75
                                    }
                                    PropertyAnimation {
                                        target: col1Rect
                                        property: "color"
                                        to: "white"
                                        duration: 0
                                    }
                                    PropertyAnimation {
                                        target: col1Rect
                                        property: "color"
                                        to: "white"
                                        duration: 75
                                    }
                                    PropertyAnimation {
                                        target: col1Rect
                                        property: "color"
                                        to: Global.selectedColor
                                        duration: 0
                                    }
                                    PropertyAnimation {
                                        target: col1Rect
                                        property: "color"
                                        to: Global.selectedColor
                                        duration: 75
                                    }
                                    PropertyAnimation {
                                        target: col1Rect
                                        property: "color"
                                        to: "white"
                                        duration: 0
                                    }
                                    PropertyAnimation {
                                        target: col1Rect
                                        property: "color"
                                        to: "white"
                                        duration: 75
                                    }
                                    PropertyAnimation {
                                        target: col1Rect
                                        property: "color"
                                        to: Global.selectedColor
                                        duration: 0
                                    }
                                    onStopped: {
                                        if (col1Rect.state == "unselected")
                                            col1Rect.color = model.col1Done ? ((model.type === Node.Group && model.level > 0) ? "#E1FFE1" : "white") : "#FFD3D3"
                                    }
                                }

                                states: [
                                    State {
                                        name: "unselected"
                                        PropertyChanges {
                                            target: col1Rect
                                            color: model.col1Done ? ((model.type === Node.Group && model.level > 0) ? "#E1FFE1" : "white") : "#FFD3D3"
                                        }
                                    },
                                    State {
                                        name: "selected"
                                        PropertyChanges {
                                            target: col1Rect
                                            color: Global.selectedColor
                                        }
                                    }
                                ]
                            }
                        }

                        states: [
                            State {
                                name: "collapsed"
                                PropertyChanges {
                                    target: subArrow
                                    rotation: 0
                                }
                            },
                            State{
                                name: "expanded"
                                PropertyChanges {
                                    target: subArrow
                                    rotation: 90
                                }
                            }
                        ]

                        transitions: [
                            Transition {
                                NumberAnimation { target: subArrow; property: "rotation"; duration: 100 }
                            }
                        ]
                        move: Transition {
                            NumberAnimation { property: "y"; duration: 100 }
                        }
                        Repeater {
                            id: objRepeater
                            model: subNodes
                            delegate: objRecursiveDelegate
                        }
                    }
                }
            }
        }
    }
}
