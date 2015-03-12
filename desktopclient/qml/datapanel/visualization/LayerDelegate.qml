import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Component{
    id : layerDelegate
    Item {
        id : layerRow
        function iconSource(name) {
            if ( name === "")
                name = "redbuttonr.png"
             var iconP = "../../images/" + name
             return iconP

         }

        width: parent.width
        height: 18

        Row {
            spacing: 2
            width : parent.width
            CheckBox{
                id : visibilityCheck
                width : 17
                height: 17
                checked : true

                style: CheckBoxStyle {
                    indicator: Rectangle {
                            implicitWidth: 14
                            implicitHeight: 14
                            radius: 1
                            border.color: control.activeFocus ? "darkblue" : "gray"
                            border.width: 1
                            Rectangle {
                                visible: control.checked
                                color: "#3333A2"
                                border.color: "#333"
                                radius: 1
                                anchors.margins: 3
                                anchors.fill: parent
                            }
                    }
                }
                opacity : index == 0 ? 0 : 1
                enabled : index == 0 ? false : true
            }

            Image {
                id : image
                width : 16; height :16
                source : iconSource(iconPath)
                fillMode: Image.PreserveAspectFit
            }
            Text{
                text : name
                width : parent.width - visibilityCheck.width - image.width
                font.pointSize: 8
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        layersList.currentIndex = index
                        layerIndex = index;
                        layersList.setEditors()
                    }
                }
            }

        }
    }
}

