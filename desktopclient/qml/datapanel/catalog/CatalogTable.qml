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

    property color background1 : "#EAECEE"
    property color background2 : "#FFF9E3"
    property int buttonSize : 55

    signal showObject(string objectid )

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
    color : "#EAECEE"
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
                width : 20;height : 20
                MouseArea {
                    id : mouseArea
                    width : 20; height : 20
                    anchors.centerIn: parent
                    drag.target: image
                    onReleased: {
                        image.Drag.drop()
                        image.parent = mouseArea
                        image.anchors.fill = mouseArea
                    }

                    Image{
                        id : image
                        width : 20; height : 20
                        source : "../../images/" + styleData.value
                        fillMode: Image.PreserveAspectFit
                        property string message :  model !== null ? model[styleData.row].url : ""

                        Drag.keys: [ model[styleData.row].iconPath ]
                        Drag.active: mouseArea.drag.active
                        Drag.hotSpot.x: 10
                        Drag.hotSpot.y: 10
                        opacity : Drag.active / 2


                        states: State {
                            when: mouseArea.drag.active
                            ParentChange { target: image; parent: root }
                            AnchorChanges { target: image; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
                        }
                    }
                    z : 1
                }
                Image{
                    id : imageBase
                    x : mouseArea.x
                    y : mouseArea.y
                    width : 20; height : 20
                    source : iconSource(styleData.value)
                    fillMode: Image.PreserveAspectFit
                    z : 0
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

        rowDelegate: Rectangle {
            id : rowdelegate
            height : 20
            color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")
        }

        onClicked: {
            var ids = ""
            resourcetable.selection.forEach( function(rowIndex) {if ( ids !== "") ids = ids + "|" ;ids = ids + (model[rowIndex].id).toString()} )
            var catalog = mastercatalog.selectedCatalog()
            if ( catalog !== null){
                catalog.setSelectedObjects(ids)
            }
        }
        onDoubleClicked: {
            if ( currentRow != -1)
                showObject(model[currentRow].id)
        }

        model : mastercatalog.resources
    }

}
