import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.1

import "../../../../qml/Global.js" as Global

Rectangle {
    anchors.fill: parent
    signal selNodeChanged (string node)

    ScrollView {
        anchors.fill: parent

        Column {
            TableView {
                height: 17
                width: Math.max(parent.parent.parent.width, criteriaTree.width + col1.width + 2)
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

            Column {
                ListView {
                    id: objView
                    model: objModel
                    width: childrenRect.width
                    height: childrenRect.height
                    delegate: objRecursiveDelegate
                }
                Button {
                    text: "Add node"
                    onClicked: {
                        objModalInput.show()
                    }
                }
            }

            ListModel {
                id: objModel
                property Rectangle selectedRow: null

                function setGoal(description, outFile) {
                    objModel.clear(); // only one goal for the tree
                    objModel.append({id : 0, type : "Goal", name : description, weight: -1, parent : objModel, level : 0, subNodes : [], fileName : outFile})
                    return objModel.get(0)
                }

                function addMask(node, description, inFile) {
                    node.subNodes.append({id : node.subNodes.count, type : "MaskArea", name : description, weight: -1, parent : node, level : 1, subNodes : [], fileName : inFile})
                }

                function addGroup(node, description, weight, outFile) {
                    node.subNodes.append({id : node.subNodes.count, type : "Group", weight: weight, name : description, parent : node, level : node.level + 1, subNodes : [], fileName : outFile})
                    return node.subNodes.get(node.subNodes.count - 1)
                }

                function addFactor(node, description, weight, inFile) {
                    node.subNodes.append({id : node.subNodes.count, type : "Factor", weight: weight, name : description, parent : node, level : node.level + 1, subNodes : [], fileName : inFile})
                }

                function addConstraint(node, description, weight, inFile) {
                    node.subNodes.append({id : node.subNodes.count, type : "Constraint", weight: weight, name : description, parent : node, level : node.level + 1, subNodes : [], fileName : inFile})
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

                    function toggleNode() {
                        var enabled
                        if (objDisplayRowRect.state == "expanded") {
                            objDisplayRowRect.state = "collapsed"
                            enabled = false
                        } else {
                            objDisplayRowRect.state = "expanded"
                            enabled = true
                        }
                        for(var i = 0; i < children.length; ++i) {
                            if(children[i].treeChild) {
                                children[i].visible = enabled
                            }
                        }
                    }

                    MouseArea {
                        id: objMouseArea
                        clip: true
                        width: criteriaTree.width
                        height: childrenRect.height
                        onDoubleClicked: {
                            if (model.type == "Constraint" || model.type == "Factor" || model.type == "MaskArea") {
                                openMap(model.fileName)
                            } else {
                                toggleNode()
                            }
                        }
                        onPressed: {
                            if (objModel.selectedRow != null) {
                                objModel.selectedRow.state = "unselected"
                            }
                            objTextRowRect.state = "selected"
                            objModel.selectedRow = objTextRowRect
                        }

                        Row {
                            id: objRow
                            Item {
                                id: objIndentation
                                height: 20
                                width: model.level * 30
                            }
                            Rectangle {
                                id: objDisplayRowRect
                                width: childrenRect.width
                                height: childrenRect.height
                                state: "expanded"

                                Row {
                                    function getIcon(nodetype) {
                                        if (nodetype === "Goal")
                                            return "Goal.png"

                                        if (nodetype === "Constraint")
                                            return "Constraint.png"

                                        if (nodetype === "Factor")
                                            return "Factorplus.png"

                                        if (nodetype === "Group")
                                            return "Objective.png"

                                        if (nodetype === "MaskArea")
                                            return "raster.png"
                                    }
                                    Image {
                                        id: subArrow
                                        width:  15
                                        height:  15
                                        //anchors.left: parent.left
                                        //anchors.leftMargin: 10
                                        //anchors.verticalCenter: parent.verticalCenter
                                        source: "arrowrightlight.png"
                                        rotation: 90
                                        visible: objRepeater.count > 0
                                        MouseArea {
                                            id: objMouseAreaArrow
                                            width: subArrow.implicitWidth
                                            height: subArrow.implicitHeight
                                            onPressed: {
                                                if (objModel.selectedRow != null) {
                                                    objModel.selectedRow.state = "unselected"
                                                }
                                                objTextRowRect.state = "selected"
                                                objModel.selectedRow = objTextRowRect
                                                toggleNode()
                                            }
                                        }
                                    }

                                    Image {
                                        id: icon
                                        //anchors.left: subArrow.right
                                        //anchors.leftMargin: 10
                                        //anchors.verticalCenter: parent.verticalCenter
                                        source: parent.getIcon(model.type)
                                        fillMode: Image.Pad
                                    }

                                    Rectangle {
                                        id: objTextRowRect
                                        width: childrenRect.width
                                        height: childrenRect.height
                                        state: "unselected"
                                        Row {

                                            Text {
                                                id: objNodeWeight
                                                text: qsTr("  " + ((model.weight >= 0) ? model.weight.toFixed(2).toString() : "") + "  ")
                                                color: "black"
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            Text {
                                                id: objNodeName
                                                text: qsTr(model.name)
                                                color: "black"
                                                verticalAlignment: Text.AlignVCenter
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
                            }
                        }
                    }
                    Repeater {
                        id: objRepeater
                        model: subNodes
                        delegate: objRecursiveDelegate
                    }
                    move: Transition {
                        NumberAnimation { property: "y"; duration: 100 }
                    }
                    Component.onCompleted: {
                        objView.width = Math.max(objView.width, width)
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
            objModel.addConstraint(group, "Inside an embankment a Greenbelt is not necessary.", 0.33, "Embanked_areas_dist_sub.mpr")
            objModel.addConstraint(group, "If the distance of an embankment to the shore inside a Greenbelt is less than 1000m there is no need for a Greenbelt, otherwise there is full priority to develop the Greenbelt", 0.33, "Embanked_areas_dist_sub.mpr")
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
