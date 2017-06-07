import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
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
            role : "iconPath"
            title : ""
            width : 25
            delegate :
                Item {
                id : imageItem
                width : 21;height : 21
                MouseArea {
                    id : mouseArea
                    width : 21; height : 21
                    anchors.centerIn: parent
                    property variant image
                    drag.target: image
                    onPressed: {
                       image = Qt.createQmlObject('import QtQuick 2.0; Image{
                            id : image
                            width : 20; height : 20
                            source : "../../images/" + styleData.value
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
                Image{
                    id : imageBase
                    x : mouseArea.x
                    y : mouseArea.y
                    width : 21; height : 21
                    source : iconSource(styleData.value)
                    fillMode: Image.PreserveAspectFit
                    z : 0
                }
            }
        }

        TableViewColumn{
            id : nameColumn
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
            id : dimColumn
            role : "dimensions"
            title : qsTr("Dimensions")
            width : 150
            delegate: Component{
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 11
                    color : styleData.selected ? "white" : "black"
                    elide: Text.ElideMiddle
                }
            }
        }

        TableViewColumn{
            id : domColumn
            role : "domainName"
            title : qsTr("Domain")
            width : 100
            delegate: Component{
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 11
                    color : styleData.selected ? "white" : "black"
                    elide: Text.ElideMiddle
                }
            }
        }

        TableViewColumn{
            id : domTypeColumn
            role : "domainType"
            title : qsTr("Domain type")
            width : 100
            delegate: Component{
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 11
                    color : styleData.selected ? "white" : "black"
                    elide: Text.ElideMiddle
                }
            }
        }

        TableViewColumn{
            id : csyColumn
            role : "coordinateSystemName"
            title : qsTr("Coordinate system")
            width : 120
            delegate: Component{
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 11
                    color : styleData.selected ? "white" : "black"
                    elide: Text.ElideMiddle
                }
            }
        }

        TableViewColumn{
            id : grfColumn
            role : "geoReferenceName"
            title : qsTr("Georeference")
            width : 120
            delegate: Component{
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 11
                    color : styleData.selected ? "white" : "black"
                    elide: Text.ElideMiddle
                }
            }
        }
        TableViewColumn{
            role : "description"
            title : qsTr("Description")
            width : parent.width - grfColumn.width - csyColumn.width - nameColumn.width - imageColumn.width - dimColumn.width - domTypeColumn.width - domColumn.width - 20
            delegate: Component{
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 11
                    color : styleData.selected ? "white" : "black"
                    elide: Text.ElideMiddle
                }
            }
        }

        rowDelegate: Rectangle {
            id : rowdelegate
            height : 23
            color : styleData.selected ? Global.selectedColor : (styleData.alternate? uicontext.lightestColor: "white")
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
                catalogViews.showObject(model[currentRow].id)

            }
        }

        model : setResources()
    }

}
