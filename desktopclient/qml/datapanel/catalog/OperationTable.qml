import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.0
import MasterCatalogModel 1.0
import CatalogModel 1.0
import ResourceModel 1.0

import "../../Global.js" as Global

Rectangle {
    id : tabPanel

    property int buttonSize : 55

    signal showObject(string objectid)
    color : tabmodel && tabmodel.side == "right" ? Global.alternatecolor3 : "white"

    function iconSource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

         var iconP = "../../images/" + name
         return iconP
     }

    width : parent.width
    anchors.margins: 2
    border.width: 1
    TableView{
        id : resourcetable
        anchors.fill: parent
        selectionMode : SelectionMode.ExtendedSelection

        TableViewColumn{
            id : imageColumn
            role : "outParameterIconList"
            title : ""
            width : 20
            delegate :
                Item {
                id : imageItem
                width : 20;height : 20
                MouseArea {
                    id : mouseArea
                    width : 20; height : 20
                    anchors.centerIn: parent
                    property variant image
                    drag.target: image
                    onPressed: {
                       image = Qt.createQmlObject('import QtQuick 2.0; Image{
                            id : image
                            width : 20; height : 20
                            source : "../../images/operation20.png"
                            fillMode: Image.PreserveAspectFit
                            property string message :  model !== null ? model[styleData.row].url : ""
                            property string ilwisobjectid : model !== null ? model[styleData.row].id : ""
                            property string type : model !== null ? typeName : ""
                            property string ids : model !== null ?  mastercatalog.selectedIds() : ""

                            Drag.keys: [ model[styleData.row].typeName ]
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
                        image.Drag.drop()
                        image.parent = mouseArea
                        image.anchors.fill = mouseArea
                        image.destroy();
                    }

                    z : 1
                }
                ListView {
                    model : styleData.value
                    //anchors.fill: parent
                    y : 10
                    orientation : ListView.Horizontal
                    delegate: Component{
                        Image{
                            width : 15
                            height : 15
                            source : iconSource(modelData)
                        }
                    }
                }
             }
        }

        TableViewColumn{
            role : "displayName"
            title : qsTr("Name")
            width : 220
            delegate : Component {
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    color : styleData.selected ? "white" : "black"
                    font.pixelSize: 12
                    font.bold: true
                    elide: Text.ElideMiddle
                }
            }

        }
        TableViewColumn{
            role : "provider"
            title : qsTr("Provider")
            width : 50
            delegate : Component {
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    color : styleData.selected ? "white" : "black"
                    font.pixelSize: 10
                    elide: Text.ElideMiddle
                }
            }

        }
        TableViewColumn{
            role : "inParameterIconList"
            title : qsTr("Input Parameters")
            width : 90
            delegate : Component {
                ListView {
                    model : styleData.value
                    //anchors.fill: parent
                    y : 5
                    orientation : ListView.Horizontal
                    delegate: Component{
                        Image{
                            width : 15
                            height : 15
                            source : iconSource(modelData)
                        }
                    }
                }
            }

        }
        TableViewColumn{
            role : "keywords"
            title : qsTr("Keywords")
            width : 180
            delegate : Component {
                TextArea {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 10
                    frameVisible: false
                    style:TextAreaStyle {
                         backgroundColor: styleData.row % 2 == 0? "#fff" : "#eee"
                     }
                }
            }

        }

        TableViewColumn{
            role : "syntax"
            title : qsTr("Syntax")
            width : 250
            delegate : Component {
                TextArea {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 10
                    frameVisible: false
                    style:TextAreaStyle {
                         backgroundColor: styleData.row % 2 == 0? "#fff" : "#eee"
                     }
                }
            }

        }

        TableViewColumn{
            role : "description"
            title : qsTr("Description")
            width : 800
            delegate : Component {
                TextArea {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 10
                    frameVisible: false
                    style:TextAreaStyle {
                         backgroundColor: styleData.row % 2 == 0? "#fff" : "#eee"
                     }
                }
            }

        }


        rowDelegate: Rectangle {
            id : rowdelegate
            height : 35
            color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")
        }

        onClicked: {
            var ids = ""
            resourcetable.selection.forEach( function(rowIndex) {if ( ids !== "") ids = ids + "|" ;ids = ids + (model[rowIndex].id).toString()} )
            mastercatalog.currentCatalog = currentCatalog
            mastercatalog.setSelectedObjects(ids)
            if (!catalogViews.tabmodel.selected)
                catalogViews.tabmodel.selectTab()
        }
        onDoubleClicked: {
            if ( currentRow != -1){
                showObject(model[currentRow].id)

            }
        }

        model : currentCatalog.operations
    }

}

