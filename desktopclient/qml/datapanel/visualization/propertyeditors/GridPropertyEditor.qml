import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls
import "../../.." as Base

Item {
    width: parent ? parent.width : 0
    height: parent ? parent.height - 10 : 0
    property var editor
    x : 5

    Column {
        y : 5
        width : parent.width - 5
        height :240
        spacing : 4
        CheckBox{
            id : primaryGridEnabled
            text : qsTr("Enabled")
            checked : editor.isActive
            style: Base.CheckBoxStyle1{}

            onCheckedChanged: {
                editor.isActive = checked
            }
        }
        Row {
            width : 150
            height : 20
            enabled : primaryGridEnabled.checked
            spacing : 4
            Text {
                width : 60
                height : 20
                text : qsTr("Cell size")
            }
            TextField{
                id : distanceValue
                width : 60
                height : 20
                text : editor.distance
            }

            Button{
                height : 18
                width : 60
                text : qsTr("Apply")
                onClicked: {
                    editor.distance = parseFloat(distanceValue.text)
                }
            }
        }

        Item {
            width : 200
            height : Global.rowHeight + 20
            property var editor

            Row {
                height : parent.height
                width : parent.width - 10
                y : 10
                spacing : 5
                TextField{
                    id : transparencyValue
                    width : 30
                    height : Global.rowHeight
                    text : editor.opacity
                    onTextChanged: {
                        if ( transparencySlider.value !== text && Global.isNumber(text)){
                            transparencySlider.value = text
                            editor.opacity = text
                        }
                    }
                }

                Slider {
                    id: transparencySlider
                    Layout.row: 2
                    value: editor.opacity
                    implicitWidth: parent.width - 40
                    style: SliderStyle { }
                    onValueChanged: {
                        if ( transparencyValue.text !== value){
                            transparencyValue.text = value.toFixed(2)
                            editor.opacity = value
                        }
                    }
                }
            }
        }

        Text{
            id : label
            text : qsTr("Grid Color")
            enabled: primaryGridEnabled.checked
        }

        Controls.ColorPicker2{
            onSelectedColorChanged: {
                editor.lineColor = selectedColor
            }
            enabled: primaryGridEnabled.checked
        }
    }

}

