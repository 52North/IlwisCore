import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../.." as Base
import "../../../controls" as Controls

Column {
    spacing : 3
    width : Math.min(300,parent ? parent.width : 300)
    height : Global.rowHeight * 2 + 20
    property var editor
    y : 10
    x : 10

    CheckBox{
        id : showBoundaries
        text: qsTr("Show")

        style: Base.CheckBoxStyle1{}
        checked : editor.showBoundaries

        onCheckedChanged: {
            editor.showBoundaries = showBoundaries.checked
        }
    }
    Row {
        height : Global.rowHeight
        width : parent.width - 10

        spacing : 5
        Text {
            id : label
            text : qsTr("Thickness")
            width : 50
        }

        TextField{
            id : thicknessValue
            width : 30
            height : Global.rowHeight
            text : "1"
            onTextChanged: {
                if ( thicknessSlider){
                    if ( thicknessSlider.value !== text && Global.isNumber(text)){
                        thicknessSlider.value = text
                        if ( editor)
                            editor.boundarywidth = text
                    }
                }
            }
        }

        Slider {
            id: thicknessSlider
            Layout.row: 2
            minimumValue: 0
            maximumValue: 10
            stepSize: 0.5
            value:1
            width: parent.width - 50 - label.contentWidth
            style: SliderStyle { }
            onValueChanged: {
                if ( thicknessValue.text !== value){
                    thicknessValue.text = value.toFixed(1)
                }
            }
        }

    }
    Row {
        height : Global.rowHeight
        width : parent.width - 10
        Text {
            id : label2
            text : qsTr("Color")
            width : 50
        }
        Controls.ColorPicker2{
            onSelectedColorChanged: {
                editor.boundaryColor = selectedColor
            }
        }
        z : 1
    }


    Row {
        height: Global.rowHeight
        width : parent.width - 10
        Text {
            id : style
            text : qsTr("Style")
            width : 50
            height: Global.rowHeight
        }
        ComboBox{
            id : stylesCombo
            height: Global.rowHeight
            width : 184
        }
    }
}

