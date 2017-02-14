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
                Button {
                    text: "Add node"
                    onClicked: {
                        console.log(Node.Factor)
                    }
                }
                Component {
                    id: objRecursiveDelegate

                    Column {
                        id: objRecursiveColumn
                        property bool treeChild: true
                        spacing: 1
                        state: "expanded"

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
                                        selectItem(objTextRowRect)
                                        selNodeChanged(model)
                                    }
                                }

                                Row {
                                    function getIcon(nodetype, level) {
                                        if (nodetype === Node.Group)
                                            if (level === 0)
                                                return "Goal.png"
                                            else
                                                return "Objective.png"
                                        else if (nodetype === Node.Constraint)
                                            return "Constraint.png"
                                        else if (nodetype === Node.Factor)
                                            return "Factorplus.png"
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
                                                selectItem(objTextRowRect)
                                                selNodeChanged(model)
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
                                                    color: "white"
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
                                    border.color = selected ? Global.edgecolor : Global.mainbackgroundcolor
                                }

                                Keys.onPressed: {
                                    switch (event.key) {
                                    case Qt.Key_F2:
                                    case Qt.Key_Enter:
                                    case Qt.Key_Return:
                                        col1MouseArea.startEdit()
                                        break
                                    case Qt.Key_Delete:
                                        model.fileName = ""
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
                                        } else
                                            startEdit()
                                    }

                                    onPressed: {
                                        saveEditAndFinish()
                                        if (selectedItem != col1Rect) {
                                            selectItem(col1Rect)
                                            selNodeChanged(null)
                                            selectionChanged = true
                                        }
                                    }

                                    onClicked: {
                                        if (selectionChanged) {
                                            selectionChanged = false
                                        } else if (clickTimer != null) {
                                            startEdit()
                                        } else
                                            clickTimer = Qt.createQmlObject("import QtQuick 2.0; Timer { id: clickTimer; interval: 500; running: true; onTriggered: {col1MouseArea.startEdit()}}", col1Rect, "clickTimer")
                                    }

                                    function startEdit() {
                                        cancelTimer()
                                        if (model.type === Node.Group)
                                            inPlaceEdit = Qt.createQmlObject("import QtQuick 2.0; import QtQuick.Controls 1.0; TextField { id: inPlaceEdit; width: parent.width; height: parent.height; text: model.fileName; verticalAlignment: TextInput.AlignVCenter; Keys.onEscapePressed: {while(canUndo) undo(); editingFinished()} onAccepted: {model.fileName = text; editingFinished()} onEditingFinished: {focus = false; visible = false; col1Rect.forceActiveFocus()} Component.onCompleted: {forceActiveFocus(); selectAll()}}", col1Rect, "inPlaceEdit");
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
                                        if (dropAllowed(drop)) {
                                            model.fileName = drop.source.message
                                        }
                                        col1MouseArea.cursorShape = Qt.ArrowCursor
                                        col1Rect.markDropCandidate(false)
                                    }
                                    onEntered: {
                                        if (dropAllowed(drag)) {
                                            col1Rect.markDropCandidate(true)
                                        } else {
                                            col1MouseArea.cursorShape = Qt.ForbiddenCursor
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
                                        visible: model.fileName !== ""
                                    }

                                    Text {
                                        id: col1NodeName
                                        text: "  " + model.fileName
                                        color: "black"
                                        verticalAlignment: Text.AlignVCenter
                                        width: col1.width - col1IconRaster.width
                                        elide: Text.ElideRight
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
                                            col1Rect.color = "white"
                                    }
                                }

                                states: [
                                    State {
                                        name: "unselected"
                                        PropertyChanges {
                                            target: col1Rect
                                            color: "white"
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

            Component.onCompleted: {
                loadSmceCatalog()
                tree.setGoal("(We want to) ... Establish biophysical priority within a potential Green Belt buffer of a maximum 1000m width along all coast lines of Bangladesh, with exception of the Sundarbans, in which the proposed Green Belt can be established to provide protection from cyclones storm surges and other natural hazards")
                tree.addMask("Analysis area")
                var group = tree.addGroup("(We want to) ... Reduce the vulnerability of population, economy and environment")
                group.addFactor("The higher the vulnerability is in the Green Belt buffer, as measured by the coastal vulnerability index, the higher the priority for development of the Green Belt")
                group = tree.addGroup("(We want to) ... Protect areas that are exposed to storm surges.")
                group.addFactor("The higher the surge hight the higher the priority to develop the Greenbelt")
                group = tree.addGroup("(We want to)...  Protect infrastructure and reduce the cost of upgrading and maintenance of infrastucture to")
                group.addConstraint("Inside an embankment a Greenbelt is not necessary.")
                group.addConstraint("If the distance of an embankment to the shore inside a Greenbelt is less than 1000m there is no need for a Greenbelt, otherwise there is full priority to develop the Greenbelt")
                group.addFactor("The closer critical infrastructure is within 20 km of the Green Belt, the higher the priority an area receives.")
                group = tree.addGroup("(We want to)... Stabilize the coastal zone and reclaim land")
                group.addFactor("Within 50 meters of an erosion area inside the Greenbelt development receives full priority whereas beyond that disatnce from erosion areas priority is none")
                group.addFactor("Accretion areas inside the Greenbelt have higher priority than other areas")
                group = tree.addGroup("(We want to).. Make use of existing forest inside the Green Belt and connect to these")
                group.addFactor("The closer to forest land inside the Green Belt, the higher the priority of the area for Green Belt development")
                group = tree.addGroup("(We want to)... Minimize the costs of land acquisition")
                group.addFactor("Public land is better than public leased land, which in turn is better than private land")
            }
        }
    }
}
