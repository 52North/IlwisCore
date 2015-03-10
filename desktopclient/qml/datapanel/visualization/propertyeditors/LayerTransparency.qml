import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global

Item {
    width : Math.min(300,parent ? parent.width : 300)
    height : Global.rowHeight + 20
    property var editor

    Row {
        height : parent.height
        width : parent.width - 10
        y : 10
        x : 10
        spacing : 5
        TextField{
            id : transparencyValue
            width : 30
            height : Global.rowHeight
            text : editor.transparency
        }

        Slider {
            id: transparencySlider
            Layout.row: 2
            value: editor.transparency
            implicitWidth: parent.width - 40
            style: SliderStyle { }
            onValueChanged: {
                transparencyValue.text = value.toFixed(2)
                editor.transparency = value
            }
        }
    }
}

