import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.1



Item {
    id: treeArea
    /*anchors.top: parent.top
    anchors.left: parent.left*/
    visible: true

    signal selNodeChanged (string node)

    Rectangle {
       id: objRoot
       objectName: "objRoot"
       width: parent.width
       height: parent.height

       ListModel {
          id: objModel
          objectName: "objModel"

          function setGoal(description, outFile) {
              objModel.clear(); // only one goal for the tree
              objModel.append({"id" : 0, "type" : "Goal", "name" : description, "weight": -1, "selected" : false, "parent" : objModel, "level" : 0, "subNodes" : [], "fileName" : outFile})
              return objModel.get(0)
          }

          function addMask(node, description, inFile) {
              node.subNodes.append({"id" : node.subNodes.count, "type" : "AArea", "name" : description, "weight": -1, "selected" : false, "parent" : node, "level" : 1, "subNodes" : [], "fileName" : inFile})
          }

          function addGroup(node, description, weight, outFile) {
              node.subNodes.append({"id" : node.subNodes.count, "type" : "Group", "weight": weight, "name" : description, "selected" : false, "parent" : node, "level" : node.level + 1, "subNodes" : [], "fileName" : outFile})
              return node.subNodes.get(node.subNodes.count - 1)
          }

          function addFactor(node, description, weight, inFile) {
              node.subNodes.append({"id" : node.subNodes.count, "type" : "Factor", "weight": weight, "name" : description, "selected" : false, "parent" : node, "level" : node.level + 1, "subNodes" : [], "fileName" : inFile})
          }

          function addConstraint(node, description, weight, inFile) {
              node.subNodes.append({"id" : node.subNodes.count, "type" : "Constraint", "weight": weight, "name" : description, "selected" : false, "parent" : node, "level" : node.level + 1, "subNodes" : [], "fileName" : inFile})
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
             objectName: "objRecursiveColumn"
             property int m_iIndex: model.index
             property var m_parentModel: model.parentModel
             property alias objMouseArea: objMouseArea
             property int selectedNode: -1

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
                    if(children[i].objectName == "objRecursiveColumn") {
                       children[i].visible = enabled
                    }
                 }
             }

             MouseArea {
                id: objMouseArea
                objectName: "objMouseArea"
                width: objRow.implicitWidth
                height: objRow.implicitHeight
                onDoubleClicked: {
                   if (model.type == "Constraint" || model.type == "Factor" || model.type == "AArea") {
                       openMap(model.fileName)
                   } else {
                       toggleNode()
                   }
                }
                onClicked: {
                    model.selected = "true"
                    console.log(model.id, model.name + " is selected...")
                    // propagate to the form..... how????
                    selectedNode = model.id

                    console.log(selectedNode + " is selected")

                    //var selNode = objModel.getById(undefined, model.id)
                    //console.log(selNode.name)
                    //selNodeChanged(selNode.id)
                    //console.log("sent signal for: " + selNode.name)
                }

                Row {
                   id: objRow
                   Item {
                      id: objIndentation
                      height: 20
                      width: model.level * 20
                   }
                   Rectangle {
                      id: objDisplayRowRect
                      height: objNodeName.implicitHeight + 5
                      width: subArrow.width + icon.width + objNodeName.implicitWidth + 5
                      state: "expanded"

                      function getIcon(nodetype) {
                        if (nodetype === "Goal")
                            return "Goal.png"

                        if (nodetype === "Constraint")
                            return "Constraint.png"

                        if (nodetype === "Factor")
                            return "Factorplus.png"

                        if (nodetype === "Group")
                            return "Objective.png"

                        if (nodetype === "AArea")
                            return "raster.png"
                      }

                      Image {
                          id: subArrow
                          width:  15
                          height:  15
                          anchors.left: parent.left
                          anchors.leftMargin: 10
                          anchors.verticalCenter: parent.verticalCenter
                          source: "arrowrightlight.png"
                          rotation: 90
                          visible: objRepeater.count > 0
                          MouseArea {
                             id: objMouseAreaArrow
                             objectName: "objMouseAreaArrow"
                             width: subArrow.implicitWidth
                             height: subArrow.implicitHeight
                             onClicked: toggleNode()
                         }
                      }

                      Image {
                          id: icon
                          width:  15
                          height:  15
                          anchors.left: subArrow.right
                          anchors.leftMargin: 10
                          anchors.verticalCenter: parent.verticalCenter
                          source: parent.getIcon(model.type)
                      }


                      Text {
                         id: objNodeWeight
                         anchors { left: icon.right; top: parent.top; bottom: parent.bottom }
                         //text: model.type + " -> " + model.weight + " " + model.name
                         text: qsTr("  " + ((model.weight >= 0) ? model.weight.toFixed(2).toString() : "") + "  ")
                         color: "black"
                         verticalAlignment: Text.AlignVCenter
                      }

                      Text {
                         id: objNodeName
                         anchors { left: objNodeWeight.right; top: parent.top; bottom: parent.bottom }
                         //text: model.type + " -> " + model.weight + " " + model.name
                         text: qsTr(model.name)
                         color: "black"
                         verticalAlignment: Text.AlignVCenter
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
                objectName: "objRepeater"
                model: subNodes
                delegate: objRecursiveDelegate
             }
          }
       }

       ColumnLayout {
          objectName: "objColLayout"
          anchors.fill: parent
          ScrollView {
             Layout.fillHeight: true
             Layout.fillWidth: true
             ListView {
                objectName: "objListView"
                model: objModel
                delegate: objRecursiveDelegate
                interactive: false
             }
          }

          Window {
             id: objModalInput
             objectName: "objModalInput"
             modality: Qt.ApplicationModal
             visible: false
             height: 200
             width: 200
             color: "white"

             TextInput {
                 id:inputweight
                 anchors.fill: parent
                 font { bold: true; pixelSize: 20 }
                 verticalAlignment: TextInput.AlignVCenter
                 horizontalAlignment: TextInput.AlignHCenter
                 focus: true
                 text: qsTr("0.5")
                 // ???
                 onFocusChanged: {
                    if(focus) {
                       selectAll()
                    }
                 }
             }


             TextInput {
                 id:inputtype
                 anchors.fill: parent
                 font { bold: true; pixelSize: 20 }
                 verticalAlignment: TextInput.AlignTop
                 horizontalAlignment: TextInput.AlignHCenter
                 focus: true
                 text: qsTr("Group")
                 // ???
                 onFocusChanged: {
                    if(focus) {
                       selectAll()
                    }
                 }
             }

             TextInput {
                id:textinput
                anchors.fill: parent
                font { bold: true; pixelSize: 20 }
                verticalAlignment: TextInput.AlignBottom
                horizontalAlignment: TextInput.AlignHCenter

                onFocusChanged: {
                   if(focus) {
                      selectAll()
                   }
                }

                function add() {
                    var szSplit = text.split(',')
                    if(szSplit.length === 1) {
                       objModel.append({"type":inputtype.text , "weight": inputweight.text,    "name": szSplit[0], "selected": "false", "level": 0, "parentModel": objModel, "subNode": []})
                    }
                    else {
                       if(objModel.get(parseInt(szSplit[0])) === undefined) {
                          console.log("Error - Given node does not exist !")
                          return
                       }
                       var node = objModel.get(parseInt(szSplit[0]))
                       for(var i = 1; i < szSplit.length - 1; ++i) {
                          if(node.subNode.get(parseInt(szSplit[i])) === undefined) {
                             console.log("Error - Given node does not exist !")
                             return
                          }
                          node = node.subNode.get(parseInt(szSplit[i]))
                       }
                       node.subNode.append({"type":inputtype.text , "weight": inputweight.text,    "name": szSplit[i], "selected": "false", "level": i, "parentModel": node.subNode, "subNode": []})
                    }
                }

                 text: qsTr("node0")
                onAccepted: {
                   if(acceptableInput) {
                      objModalInput.close()
                       add()
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

          Button {
             text: "Add node"
             onClicked: {
                objModalInput.show()
             }
          }
       }
    }
}
