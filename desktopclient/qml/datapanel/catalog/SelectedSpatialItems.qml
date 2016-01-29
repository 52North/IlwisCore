import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../visualization" as MapTools
import "../../Global.js" as Global

GridView {
    id : thumbgrid
    width: 240
    height: 280
    cellHeight: 75
    cellWidth: 100

    function iconSource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( !name || name === "?")
            name = "redbuttonr.png"

         var iconP = "../../images/" + name
         return iconP

     }

    delegate :
        Rectangle {
            height : cellHeight
            width : cellWidth
            border .width: 5
            border.color: "lightgrey"
            Rectangle {
                id : frame
                y : 2
                x : 2
                width : parent.width -5
                height :cellHeight - 4
                color : thumbgrid.currentIndex === index ? Global.selectedColor : Global.alternatecolor3
                Image {
                    id: img
                    width : parent.width -5
                    height : 52
                    x : 5
                    y : 2

                    source: iconSource(modelData.imagePath)
                    fillMode: Image.PreserveAspectFit
                    asynchronous: true
                }

                Text {
                    anchors.top: img.bottom
                    anchors.topMargin: 2
                    width : parent.width -5
                    height : 20
                    elide : Text.ElideMiddle
                    text : modelData.name
                    clip : true
                    anchors.horizontalCenter: img.horizontalCenter

                }
                MouseArea{
                    id : mouseArea
                    anchors.fill : parent
                    property variant image
                    drag.target: image
                    onClicked: {
                        thumbgrid.currentIndex = index
                        setSelected(modelData.id)
                    }
                    onDoubleClicked: {
                         showObject(modelData.id)
                        thumbgrid.visible = false
                        thumbgrid.enabled = false
                    }
                    onReleased: {
                        image.Drag.drop()
                        image.parent = mouseArea
                        image.anchors.fill = mouseArea
                        image.destroy();
                    }
                    onPressed: {
                       image = Qt.createQmlObject('import QtQuick 2.0; Image{
                            id : image
                            width : 20; height : 20
                            source : iconSource(modelData.iconPath)
                            fillMode: Image.PreserveAspectFit
                            property string message :  model !== null ? modelData.url : ""
                            property string ilwisobjectid : model !== null ? modelData.id : ""
                            property string type : model !== null ? modelData.typeName : ""
                            property string ids : model !== null ? mastercatalog.selectedIds() : ""

                            Drag.keys: modelData.typeName
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
                }
            }

    }

}

