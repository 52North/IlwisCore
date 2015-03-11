import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global

Item {
    width : Math.min(300,parent ? parent.width : 300)
    height : Global.rowHeight * 2 + 20
    property var editor
    y : 10
    x : 10

    Column {

        spacing : 5
        width : parent.width
        height : parent.height - 10
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
                onTextChanged: {
                    if ( thicknessSlider.value !== text && Global.isNumber(text)){
                        thicknessSlider.value = text
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
            height: Global.rowHeight
            width : parent.width - 10
            Text {
                id : style
                text : qsTr("Style")
                width : 50
            }
            ComboBox{
                width : 150
                height : Global.rowHeight
                id : stylesCombo
            }


        }
    }
}

