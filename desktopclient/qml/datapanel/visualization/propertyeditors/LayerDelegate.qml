import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global
import "../../.." as Base

Component{
    id : layerDelegate
    Item {
        id : layerRow
        function iconSource(name) {
            if ( name === "")
                name = "redbuttonr.png"
             var iconP = "../../../images/" + name
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
                checked : manager.layer(index).active

                style: Base.CheckBoxStyle1{}

                opacity : index == 0 ? 0 : 1
                enabled : index == 0 ? false : true

                onCheckedChanged: {
                    manager.layer(index).active = checked
                }
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
                elide: Text.ElideMiddle
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        layersList.currentIndex = index
                        attributeListColumn.currentCoverage = manager.layer(index)

                    }
                }
            }

        }
    }
}

