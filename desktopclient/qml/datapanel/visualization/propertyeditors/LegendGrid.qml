import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import RepresentationElement 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Item {
    function setColor(clr){
        if ( grid.currentIndex != -1){
            grid.model[grid.currentIndex].ecolor = clr
            var expr = "attributefillcolor(" + renderer.viewerId +"," + editor.layerIndex + "," + editor.attributeName + "," + grid.model[grid.currentIndex].label + "," + clr +")"
            renderer.addCommand(expr)
            renderer.update()
        }
    }

    Controls.TextEditLabelPair{
        id : rprName
        content : editor.representationName
        labelText: qsTr("Name")
        labelWidth: 100
    }
    ScrollView {
        id : scroller
        width : parent.width - 5
        height : parent.height - rprName.height - 4
        anchors.top : rprName.bottom
        x : 5



        GridView{
            id : grid
            anchors.fill: parent
            anchors.topMargin: 4
            property int maxChar : 15

            model : editor.representationElements
            onModelChanged: {
                var maxl = 0
                for(var i = 0; i < model.length; ++i)    {
                    maxl = Math.max(maxl, model[i].label.length)
                }
                maxChar = maxl
            }

            cellWidth: 45 + Math.max(30, maxChar * 6)
            cellHeight: 23
            flow: GridView.FlowTopToBottom
            highlight: Rectangle { color: Global.selectedColor; radius: 2 }
            delegate: Component{
                Item{
                    width :  GridView.view.cellWidth
                    height :  GridView.view.cellHeight
                    Row{
                        width : parent.width
                        height : 20
                        anchors.verticalCenter: parent.verticalCenter
                        spacing : 5
                        Rectangle{
                            id : colorrect
                            width : 40
                            height : 20
                            color : ecolor
                            border.width: 1
                            border.color : "grey"
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Text{
                            height : 20
                            width : parent.width - colorrect.width - 10
                            text : label
                            y : 4
                            clip : true
                            elide: Text.ElideMiddle
                        }

                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            var isChanged = index != grid.currentIndex
                            grid.currentIndex = index
                            if (legend.state == "visible" && !isChanged){
                                legend.state = "invisible"
                                grid.currentIndex = -1
                            }
                            else
                                legend.state = "visible"
                        }
                    }
                }
            }
            Component.onCompleted: {
                currentIndex = -1
            }
        }
    }
}

