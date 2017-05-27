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
    property string nonselectColor : uicontext.paleColor
    property int tabHeight : 35

    tab: BorderImage {
        id: name
        source: tabmodel && tabmodel.selected ? "images/tab_big_active2.png" : "images/tab_big_inactive2.png"
        border.left: 5; border.right: 5

        property TabModel tabmodel : datapane.tab(side == 1, styleData.index)

        width: 300
        height: tabHeight

        Item {
            id : label
            width : parent.width - closeButton.width - expandButton.width
            height : tabHeight - 2
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            Row {
                width : parent.width - 10
                anchors.bottom: parent.bottom
                x : 8
                height : 35

                Text {
                    width : parent.width - 25
                    height : 20
                    text: tabmodel ? tabmodel.displayName : "?"
                    color: tabmodel && tabmodel.selected ? "black" : "#403F3F"
                    elide: Text.ElideMiddle
                    font.pointSize: tabmodel && tabmodel.selected ? 9 : 8
                    font.weight: tabmodel && tabmodel.selected ? Font.Bold : Font.Normal
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 2
                }

                Text {
                    width : 20
                    height : 14
                    text: tabmodel ? "[" + tabmodel.id + "]": "?"
                    color: tabmodel && tabmodel.selected ? "black" : "#403F3F"
                    font.pointSize: 8
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 8
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
                    dataPanel.activeItem = control.getTab(currentIndex).item
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
            y : 6
            anchors.right : parent.right
            anchors.rightMargin: 6
            width : 18
            height : width
            opacity : datapanesplit.totalTabCount() > 1 ? 0.5 : 0
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
            y : 6
            anchors.right : closeButton.left
            anchors.rightMargin: 4
            width : 18
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
