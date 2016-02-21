import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../controls" as Controls

Controls.DropableItem{
    id : dropItem
    width : 275
    height : 0
    clip:true
    property string choice
    GroupBox {
        title: "Projected Coordinate system create methods"
        ColumnLayout {
            ExclusiveGroup {
                id: group
                onCurrentChanged: {
                    if( numericdomain.checked){
                        choice = "numericdomain"
                    }
                    if( timedomain.checked){
                        choice = "timedomain"
                    }
                    if( identifierdomain.checked){
                        choice = "nameidentifierdomain"
                    }
                    if( thematicdomain.checked){
                        choice = "thematicdomain"
                    }
                    if( intervaldomain.checked){
                        choice = "intervaldomain"
                    }
                    if( palettedomain.checked){
                        choice = "palettedomain"
                    }
                    if( continousclrd.checked){
                        choice = "contcolordomain"
                    }
                }
            }
            RadioButton {
                id : numericdomain
                text: qsTr("Numeric Domain")
                checked: true
                exclusiveGroup: group
            }
            RadioButton {
                id : timedomain
                text: qsTr("Time Domain")
                exclusiveGroup: group
            }
            RadioButton {
                id : identifierdomain
                text: qsTr("Identifier Domain")
                exclusiveGroup: group
            }
            RadioButton {
                id : thematicdomain
                text: qsTr("Thematic Domain")
                exclusiveGroup: group
            }
            RadioButton {
                id : intervaldomain
                text: qsTr("Interval Domain")
                exclusiveGroup: group
            }
            RadioButton {
                id : palettedomain
                text: qsTr("Palette Domain")
                exclusiveGroup: group
            }
            RadioButton {
                id : continousclrd
                text: qsTr("Continious Color Domain")
                exclusiveGroup: group
            }
        }
    }
    Item {
        width : parent.width
        height : 30
        anchors.bottom : parent.bottom
        anchors.bottomMargin: 8
        anchors.rightMargin: 3
        Button {
            id : applybutton
            anchors.right: parent.right
            width : 70
            text : qsTr("Next")
            y : 10
            onClicked: {
                editorList.pop()
                 editorList.push({item:creatorContainer, properties:{creator:objectcreator.creatorInfo(choice)}})
            }

        }
        Button {
            id : closebutton
            anchors.right: applybutton.left
            anchors.rightMargin: 5
            width : 70
            text : qsTr("Close")
            y : 10

            onClicked: {
                editorList.pop()
            }

        }
    }

}

