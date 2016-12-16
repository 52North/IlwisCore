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
              if(szSplit.length === 1) {
                 objModel.append({"id" : idcounter++, "type": "", "weight": "", "name": szSplit[0], "selected": "false", "level": 0, "parentModel": objModel, "subNode": []})
              }
              else {
                 if(objModel.get(parseInt(szSplit[0])) === undefined) {
                    console.log("1 - Error - Given node does not exist !")
                    return
                 }
                 var node = objModel.get(parseInt(szSplit[0]))
                 for(var i = 1; i < szSplit.length - 1; ++i) {
                    if(node.subNode.get(parseInt(szSplit[i])) === undefined) {
                       console.log("2 - Error - Given node does not exist !")
                       return
                    }
                    node = node.subNode.get(parseInt(szSplit[i]))
                 }
                 node.subNode.append({"id" : idcounter++, "type": "", "weight": "", "name": szSplit[i], "selected": "false", "level": i, "parentModel": node.subNode, "subNode": []})
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
                      width: subArrow.width + objNodeName.implicitWidth + 5

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

                      Text {
                         id: objNodeName
                         anchors { left: subArrow.right; top: parent.top; bottom: parent.bottom }
                         text: model.type + " -> " + model.weight + " " + model.name
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
              objModel.add2("(We want to) ... Establish biophysical priority within a potential Green Belt buffer of a maximum 1000m width along all coast lines of Bangladesh, with exception of the Sundarbans, in which the proposed Green Belt can be established to provide protection from cyclones storm surges and other natural hazards")
              objModel.add2("0---Analysis area")
              objModel.add2("0---(We want to) ... Reduce the vulnerability of population, economy and environment")
              objModel.add2("0---1---The higher the vulnerability is in the Green Belt buffer, as measured by the coastal vulnerability index, the higher the priority for development of the Green Belt")
              objModel.add2("0---(We want to) ... Protect areas that are exposed to storm surges.")
              objModel.add2("0---(We want to)...  Protect infrastructure and reduce the cost of upgrading and maintenance of infrastucture to")
              objModel.add2("0---3---The closer a location inside the Greenbelt is to an embankment inside the greenbelt the higher the priority of greenbelt development in order to provide protection to of the embankment")
              objModel.add2("0---3---The closer critical infrastructure is within 20 km of the Green Belt, the higher the priority an area receives.")
              objModel.add2("0---(We want to)... Stabilize the coastal zone and reclaim land")
              objModel.add2("0---(We want to).. Make use of existing forest inside the Green Belt and connect to these")
              objModel.add2("0---(We want to)... Minimize the costs of land acquisition")


              objModel.traverse()
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
