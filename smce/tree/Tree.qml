import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.1

import "../../../../qml/Global.js" as Global

Rectangle {
    anchors.fill: parent
    signal selNodeChanged(Text node)

    MouseArea {
        anchors.fill: parent
        onPressed: {
            if (objModel.selectedItem != null) {
                objModel.selectedItem.state = "unselected"
            }
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
                    model: objModel
                    width: criteriaTree.width + col1.width
                    height: childrenRect.height
                    delegate: objRecursiveDelegate
                }
                Button {
                    text: "Add node"
                    onClicked: {
                        objModalInput.show()
                    }
                }
                ListModel {
                    id: objModel
                    property Rectangle selectedItem: null

                    function setGoal(description, outFile) {
                        objModel.clear(); // only one goal for the tree
                        objModel.append({id : 0, type : "Group", name : description, weight: -1, parent : null, subNodes : [], fileName : outFile})
                        return objModel.get(0)
                    }

                    function addMask(node, description, inFile) {
                        node.subNodes.append({id : node.subNodes.count, type : "MaskArea", name : description, weight: -1, parent : node, subNodes : [], fileName : inFile})
                    }

                    function addGroup(node, description, weight, outFile) {
                        node.subNodes.append({id : node.subNodes.count, type : "Group", weight: weight, name : description, parent : node, subNodes : [], fileName : outFile})
                        return node.subNodes.get(node.subNodes.count - 1)
                    }

                    function addFactor(node, description, weight, inFile) {
                        node.subNodes.append({id : node.subNodes.count, type : "Factor", weight: weight, name : description, parent : node, subNodes : [], fileName : inFile})
                    }

                    function addConstraint(node, description, inFile) {
                        node.subNodes.append({id : node.subNodes.count, type : "Constraint", weight: -1, name : description, parent : node, subNodes : [], fileName : inFile})
                    }

                    function level(node) {
                        if (node.parent != null)
                            return 1 + level(node.parent)
                        else
                            return 0
                    }

                    function traverse(node) {
                        // traverse data model
                        //console.log("1 traverse:" + objModel.count)

                        if (node === undefined) {// root
                            node = objModel.get(0)
                            console.log(node.id + node.name)
                        }
                        if (node.subNodes !== undefined) {
                            //console.log("3 traverse:" + node.subNodes.count)
                            for(var i = 0; i < node.subNodes.count; ++i) {
                                var subNode = node.subNodes.get(i)
                                //console.log(subNode.id+subNode.name)
                                console.log(subNode.id+subNode.name)
                                if (subNode.subNodes !== undefined)
                                    traverse(subNode)
                            }
                        }
                    }

                    function getById(node, id) {
                        // traverse data model
                        //console.log("1 traverse:" + objModel.count)

                        if (node === undefined) {// root
                            node = objModel.get(0)
                            //console.log(node.id + node.name)
                            if (node.id === id)
                                return node
                        }
                        if (node.subNodes !== undefined) {
                            //console.log("3 traverse:" + node.subNodes.count)
                            for(var i = 0; i < node.subNodes.count; ++i) {
                                var subNode = node.subNodes.get(i)
                                console.log(subNode.id+subNode.name)
                                if (subNode.id === id) {
                                    return subNode
                                } else {
                                    if (subNode.subNodes !== undefined)
                                        return getById(subNode, id)
                                }
                            }
                        }
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
                                        if (model.type === "Group") {
                                            toggleNode()
                                        }
                                    }
                                    onPressed: {
                                        if (objModel.selectedItem != null) {
                                            objModel.selectedItem.state = "unselected"
                                        }
                                        objTextRowRect.state = "selected"
                                        objModel.selectedItem = objTextRowRect
                                        selNodeChanged(objNodeName)
                                    }
                                }

                                Row {
                                    function getIcon(nodetype, level) {
                                        if (nodetype === "Group")
                                            if (level === 0)
                                                return "Goal.png"
                                            else
                                                return "Objective.png"
                                        else if (nodetype === "Constraint")
                                            return "Constraint.png"
                                        else if (nodetype === "Factor")
                                            return "Factorplus.png"
                                        else if (nodetype === "MaskArea")
                                            return "raster.png"
                                    }
                                    Item {
                                        id: objIndentation
                                        height: 20
                                        width: objModel.level(model) * 30
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
                                                if (objModel.selectedItem != null) {
                                                    objModel.selectedItem.state = "unselected"
                                                }
                                                objTextRowRect.state = "selected"
                                                objModel.selectedItem = objTextRowRect
                                                selNodeChanged(objNodeName)
                                                toggleNode()
                                            }
                                        }
                                    }

                                    Image {
                                        id: icon
                                        source: parent.getIcon(model.type, objModel.level(model))
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

                                MouseArea {
                                    id: col1MouseArea
                                    anchors.fill: parent
                                    onDoubleClicked: {
                                        if (model.fileName !== "")
                                            openMap(model.fileName)
                                    }
                                    onPressed: {
                                        if (objModel.selectedItem != null) {
                                            objModel.selectedItem.state = "unselected"
                                        }
                                        col1Rect.state = "selected"
                                        objModel.selectedItem = col1Rect
                                        selNodeChanged(col1NodeName)
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
                var goal = objModel.setGoal("(We want to) ... Establish biophysical priority within a potential Green Belt buffer of a maximum 1000m width along all coast lines of Bangladesh, with exception of the Sundarbans, in which the proposed Green Belt can be established to provide protection from cyclones storm surges and other natural hazards", "Greenbelt_development_priority_sub.mpr")
                objModel.addMask(goal, "Analysis area", "Vulnerability_reduction_priority_sub.mpr")
                var group = objModel.addGroup(goal, "(We want to) ... Reduce the vulnerability of population, economy and environment", 0.33, "")
                objModel.addFactor(group, "The higher the vulnerability is in the Green Belt buffer, as measured by the coastal vulnerability index, the higher the priority for development of the Green Belt", 1.0, "Greenbelt_development_priority_sub.mpr")
                group = objModel.addGroup(goal, "(We want to) ... Protect areas that are exposed to storm surges.", 0.33, "")
                objModel.addFactor(group, "The higher the surge hight the higher the priority to develop the Greenbelt", 1.0, "surgeh1_positive_LT_6m_resampl_25m_sub.mpr")
                group = objModel.addGroup(goal, "(We want to)...  Protect infrastructure and reduce the cost of upgrading and maintenance of infrastucture to", 0.33, "")
                objModel.addConstraint(group, "Inside an embankment a Greenbelt is not necessary.", "Embanked_areas_dist_sub.mpr")
                objModel.addConstraint(group, "If the distance of an embankment to the shore inside a Greenbelt is less than 1000m there is no need for a Greenbelt, otherwise there is full priority to develop the Greenbelt", "Embanked_areas_dist_sub.mpr")
                objModel.addFactor(group, "The closer critical infrastructure is within 20 km of the Green Belt, the higher the priority an area receives.", 0.33, "Coastal_stabilization_reclamation_prioirty_sub.mpr")
                group = objModel.addGroup(goal, "(We want to)... Stabilize the coastal zone and reclaim land", 0.2, "Dummy_map.mpr")
                objModel.addFactor(group, "Within 50 meters of an erosion area inside the Greenbelt development receives full priority whereas beyond that disatnce from erosion areas priority is none", 0.5, "Dummy_map.mpr")
                objModel.addFactor(group, "Accretion areas inside the Greenbelt have higher priority than other areas", 0.5, "Dummy_map.mpr")
                group = objModel.addGroup(goal, "(We want to).. Make use of existing forest inside the Green Belt and connect to these", 0.1, "Dummy_map.mpr")
                objModel.addFactor(group, "The closer to forest land inside the Green Belt, the higher the priority of the area for Green Belt development", 1.0, "Dummy_map.mpr")
                group = objModel.addGroup(goal, "(We want to)... Minimize the costs of land acquisition", 0.1, "Dummy_map.mpr")
                objModel.addFactor(group, "Public land is better than public leased land, which in turn is better than private land", 1.0, "Dummy_map.mpr")

                // objModel.traverse()
            }
        }
    }
}
