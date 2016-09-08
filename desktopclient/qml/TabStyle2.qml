import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import DataPaneModel 1.0
import TabModel 1.0
import "Global.js" as Global
import "./controls" as Controls


TabViewStyle {

    signal expandToWindow(int panelside, int tabindex);
    signal fullSize()

    property string selectColor : "white"
    property string nonselectColor : Global.palegreen
    property int tabHeight : 35 * Global.uiScale

    tab: Rectangle {
        id : selectRect
        property TabModel tabmodel : datapane.tab(side == 1, styleData.index)

        color: tabmodel && tabmodel.selected ? selectColor : nonselectColor
        border.color:  Global.edgecolor

        implicitWidth: Global.uiScale *140
        implicitHeight: tabHeight
        radius: 5


        Item {
            id : label
            width : parent.width - closeButton.width - expandButton.width
            height : tabHeight - 2
            y : 1  * Global.uiScale
            x : 6 *  Global.uiScale
            Column {
                anchors.fill: parent
                Text {
                    width : parent.width
                    height : 14 * Global.uiScale
                    text: tabmodel ? "[ " + tabmodel.id + " ]": "?"
                    color: tabmodel && tabmodel.selected ? "black" : "white"
                    font.pointSize: 8 * Global.uiScale
                }

                Text {
                    width : parent.width
                    height : 20 * Global.uiScale
                    text: tabmodel ? tabmodel.displayName : "?"
                    color: tabmodel && tabmodel.selected ? "black" : "white"
                    elide: Text.ElideMiddle
                    font.pointSize: tabmodel && tabmodel.selected ? 9 : 8
                    //font.bold: tabmodel && tabmodel.selected ? true : false
                    font.weight: tabmodel && tabmodel.selected ? Font.DemiBold : Font.Normal


                }
            }
            MouseArea  {
                id : mouseArea2
                property variant image
                drag.target: image
                anchors.fill : parent
                onClicked : {
                    currentIndex = styleData.index
                    datapane.select(side == 1, currentIndex, true)
                    datatabview.sideChange()

                }
                onDoubleClicked: {
                    fullSize()
                }
                onReleased: {
                   if ( image){
                        image.Drag.drop()
                        image.parent = mouseArea2
                        image.anchors.fill = mouseArea2
                        image.destroy();
                    }else
                       showTimer.stop()
                }

                function createDragObject() {
                    image = Qt.createQmlObject('import QtQuick 2.0; Image{
                        id : image
                        width : 140; height : 35
                        source : \"images/tabborderdrag.png\"
                        fillMode: Image.PreserveAspectFit
                        property var sourcePanel : control.getTab(currentIndex).item

                        Drag.keys: "datatab"
                        Drag.active: mouseArea2.drag.active
                        Drag.hotSpot.x: 10
                        Drag.hotSpot.y: 10
                        Drag.source : image
                        opacity : 0.5

                        states: State {
                            when: mouseArea2.drag.active
                            ParentChange { target: image; parent: root }
                            AnchorChanges { target: image; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
                        }
                    }', mouseArea2, "dynamicImage");
                }

                onPressed: {
                    showTimer.start()
                }
            }
            Timer{
                id : showTimer
                interval: 250
                onTriggered: {
                    mouseArea2.createDragObject()


                }

            }
        }


        Button {
            id : closeButton
            y : 3
            anchors.right : parent.right
            anchors.rightMargin: 3
            width : 18 * Global.uiScale
            height : width
            opacity : 0.5
            onClicked: {
               datapanesplit.closeTab(side == 1, styleData.index)
            }

            Image {
                y : 2
                x : 1
                width : 15
                height : 15
                source : "images/close_grey.png"


            }
        }

        Button {
            id : expandButton
            y : 2
            anchors.right : closeButton.left
            anchors.leftMargin: 2
            width : 18 * Global.uiScale
            height : width
            opacity : canSeparate(index) ? 0.7 : 0;
            enabled : canSeparate(index)
            onClicked: expandToWindow(side,index)

            Image {
                y : 2
                x : 1
                width : 15
                height : 15
                source : "images/expand20.png"
            }
        }

    }
    frame: Rectangle { color: "white"; border.width: 1 ; border.color: "lightgrey" }
}
