import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import DataPaneModel 1.0
import TabModel 1.0
import "Global.js" as Global


TabViewStyle {

    signal expandToWindow(int panelside, int tabindex);
    signal fullSize()

    property string selectColor : "steelblue"
    property string nonselectColor : "lightsteelblue"
    property int tabHeight : 35


    tab: Rectangle {
        id : selectRect
        property TabModel tabmodel : datapane.tab(side == 1, styleData.index)

        color: tabmodel && tabmodel.selected ? "black" : nonselectColor
        border.color:  Global.edgecolor

        implicitWidth: Math.max(label.width + 4, 140)
        implicitHeight: tabHeight
        radius: 5


        Item {
            id : label
            width : parent.width - closeButton.width - expandButton.width
            height : tabHeight - 2
            y : 1
            x : 6
            Column {
                anchors.fill: parent
                Text {
                    width : parent.width
                    height : 14
                    text: tabmodel ? "[ " + tabmodel.id + " ]": "?"
                    color: tabmodel && tabmodel.selected ? "white" : "black"
                    font.pointSize: 8
                }

                Text {
                    width : parent.width
                    height : 20
                    text: tabmodel ? tabmodel.displayName : "?"
                    color: tabmodel && tabmodel.selected ? "white" : "black"
                    elide: Text.ElideMiddle
                    font.pointSize: tabmodel && tabmodel.selected ? 10 : 9
                    font.bold: tabmodel && tabmodel.selected ? true : false


                }
            }
            MouseArea  {
                anchors.fill : parent
                onClicked : {
                    currentIndex = styleData.index
                    datapane.select(side == 1, currentIndex, true)
                }
                onDoubleClicked: {
                    fullSize()
                }
            }
        }


        Button {
            id : closeButton
            y : 2
            anchors.right : parent.right
            anchors.rightMargin: 2
            width : 18
            height : 18
            opacity : 0.5
            onClicked: {
               datapanesplit.closeTab(side == 1, styleData.index)
            }

            Image {
                y : 2
                x : 1
                width : 15
                height : 15
                source : "images/close20.png"


            }
        }

        Button {
            id : expandButton
            y : 2
            anchors.right : closeButton.left
            anchors.leftMargin: 2
            width : 18
            height : 18
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
