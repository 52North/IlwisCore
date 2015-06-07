import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import DataPaneModel 1.0
import TabModel 1.0


TabViewStyle {

    signal expandToWindow(int panelside, int tabindex);

    property string selectColor : "steelblue"
    property string nonselectColor : "lightsteelblue"
    property int tabHeight : 22


    frameOverlap: 1
    tab: Rectangle {
        id : selectRect
        property TabModel tabmodel : datapane.tab(side == 1, styleData.index)

        color: tabmodel && tabmodel.selected ? selectColor : nonselectColor
        border.color:  color

        implicitWidth: Math.max(label.width + 4, 170)
        implicitHeight: tabHeight
        radius: 2
        Text {
            id: label
            width : 100
            anchors.centerIn: parent
            text: styleData.title
            color: styleData.selected ? "white" : "black"
            elide: Text.ElideMiddle

            MouseArea  {
                anchors.fill : parent
                onClicked : {
                    currentIndex = styleData.index
                    datapane.select(side == 1, currentIndex, true)
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
//            onClicked: {
//                datapanesplit.showMapWindow(0)
//            }
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
