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


    property int idcounter : 0

    signal selNodeChanged (string node)


    Rectangle {
       id: objRoot
       objectName: "objRoot"
       width: parent.width
       height: parent.height

       ListModel {
          id: objModel
          objectName: "objModel"

          function add2(nodeinfo) {
              var szSplit = nodeinfo.split('---')
              if(szSplit.length === 3 && szSplit[1] === "Goal") {
                 objModel.append({"id" : idcounter++, "type": "Goal", "weight": "", "name": szSplit[0], "selected": "false", "level": 0, "parentModel": objModel, "subNode": []})
              }
              else {
                 if(objModel.get(parseInt(szSplit[0])) === undefined) {
                    console.log("1 - Error - Given node does not exist !")
                    return
                 }
                 var node = objModel.get(parseInt(szSplit[0]))
                 for(var i = 1; i < szSplit.length - 3; ++i) {
                    if(node.subNode.get(parseInt(szSplit[i])) === undefined) {
                       console.log("2 - Error - Given node does not exist !")
                       return
                    }
                    node = node.subNode.get(parseInt(szSplit[i]))
                 }
                 node.subNode.append({"id" : idcounter++, "type": szSplit[i+1], "weight": szSplit[i+2], "name": szSplit[i], "selected": "false", "level": i, "parentModel": node.subNode, "subNode": []})
              }
          }

          function traverse(node) {
                // traverse data model
              //console.log("1 traverse:" + objModel.count)

              if (node === undefined) {// root
                node = objModel.get(0)
                console.log(node.id + node.name)
              }
              if (node.subNode !== undefined) {
                //console.log("3 traverse:" + node.subNode.count)
                for(var i = 0; i < node.subNode.count; ++i) {
                    var subNode = node.subNode.get(i)
                    //console.log(subNode.id+subNode.name)
                    console.log(subNode.id+subNode.name)
                    if (subNode.subNode !== undefined)
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
              if (node.subNode !== undefined) {
                //console.log("3 traverse:" + node.subNode.count)
                for(var i = 0; i < node.subNode.count; ++i) {
                    var subNode = node.subNode.get(i)
                    console.log(subNode.id+subNode.name)
                    if (subNode.id === id) {
                        return subNode
                    } else {
                        if (subNode.subNode !== undefined)
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
             MouseArea {
                id: objMouseArea
                objectName: "objMouseArea"
                width: objRow.implicitWidth
                height: objRow.implicitHeight
                onDoubleClicked: {
                   for(var i = 0; i < parent.children.length; ++i) {
                      if(parent.children[i].objectName !== "objMouseArea") {
                         parent.children[i].visible = !parent.children[i].visible
                         if (parent.children[i].visible) {
                             objDisplayRowRect.state = "open"
                         }
                         else
                         {
                             objDisplayRowRect.state = "collapsed"
                         }
                      }
                   }
                }
                onClicked: {
                    model.selected = "true"
                    console.log(model.id, model.name + " is selected...")
                    // propagate to the form..... how????
                    selectedNode = model.id

                    console.log(selectedNode + " is selected")

                    var selNode = objModel.getById(undefined, model.id)
                    console.log(selNode.name)
                    selNodeChanged(selNode.id)
                    console.log("sent signal for: " + selNode.name)
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
                      width: icon.width + subArrow.width + objNodeName.implicitWidth + 5

                      function getIcon(nodetype) {
                        if (nodetype === "Goal")
                            return "Goal.png"

                        if (nodetype === "Constraint")
                            return "Constraint.png"

                        if (nodetype === "Factor")
                            return "Factorplus.png"

                        if (nodetype === "Objective")
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
                         text: qsTr("  " + model.weight + "  ")
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
                              name: "open"
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
                model: subNode
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
                 text: qsTr("Objective")
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
              objModel.add2("(We want to) ... Establish biophysical priority within a potential Green Belt buffer of a maximum 1000m width along all coast lines of Bangladesh, with exception of the Sundarbans, in which the proposed Green Belt can be established to provide protection from cyclones storm surges and other natural hazards---Goal---")
              objModel.add2("0---Analysis area---AArea---")
              objModel.add2("0---(We want to) ... Reduce the vulnerability of population, economy and environment---Objective---0.33")
              objModel.add2("0---1---The higher the vulnerability is in the Green Belt buffer, as measured by the coastal vulnerability index, the higher the priority for development of the Green Belt---Factor---1.0")
              objModel.add2("0---(We want to) ... Protect areas that are exposed to storm surges.---Objective---0.33")
              objModel.add2("0---2---The higher the surge hight the higher the priority to develop the Greenbelt---Factor---1.0")
              objModel.add2("0---(We want to)...  Protect infrastructure and reduce the cost of upgrading and maintenance of infrastucture to---Objective---0.33")
              objModel.add2("0---3---Inside an embankment a Greenbelt is not necessary.---Constraint---0.33")
              objModel.add2("0---3---If the distance of an embankment to the shore inside a Greenbelt is less than 1000m there is no need for a Greenbelt, otherwise there is full priority to develop the Greenbelt---Constraint---0.33")
              objModel.add2("0---3---The closer critical infrastructure is within 20 km of the Green Belt, the higher the priority an area receives.---Factor---0.33")

              /*objModel.add2("0---(We want to)... Stabilize the coastal zone and reclaim land---Objective---0.2")
              objModel.add2("0---4---Within 50 meters of an erosion area inside the Greenbelt development receives full priority whereas beyond that disatnce from erosion areas priority is none---Factor---0.5")
              objModel.add2("0---4---Accretion areas inside the Greenbelt have higher priority than other areas---Factor---0.5")
              objModel.add2("0---(We want to).. Make use of existing forest inside the Green Belt and connect to these---Objective---0.1")
              objModel.add2("0---5---The closer to forest land inside the Green Belt, the higher the priority of the area for Green Belt development---Factor---1.0")
              objModel.add2("0---(We want to)... Minimize the costs of land acquisition---Objective---0.1")
              objModel.add2("0---6---Public land is better than public leased land, which in turn is better than private land---Factor---1.0")*/

              objModel.traverse()


              //newTab = datapanesplit.newPanel(filter, resource.typeName,resource.url,"other")
              var smceCatalog = mastercatalog.newCatalog("file:///F:/testdata2/BFD/Final_Data/SMCE_Greenbelt_submission_2711/sub","")
              //mastercatalog.currentCatalog = smceCatalog
              //smceCatalog.scanContainer(false, false)
              var resources = smceCatalog.resources
              var sz = resources.length
              console.log("start, length=" + sz)
              for(var j = 0; j < sz; ++j) {
                console.log ("name=" +resources[j].name + " id=" + resources[j].id)
                if (resources[j].name == "Coastal_stabilization_reclamation_prioirty_sub.mpr") {
                    resource = resources[j]
                    filter = "itemid=" + resources.id
                    newTab = datapanesplit.newPanel(filter, resource.typeName,resource.url,"other")
                    break;
                }
              }
              console.log("end")
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
