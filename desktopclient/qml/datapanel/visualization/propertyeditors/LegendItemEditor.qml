import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import RepresentationElement 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Rectangle {
    width: parent.width
    height: 10

    Controls.ColorPicker2{
        x : 4
        y : 4

        onSelectedColorChanged: {
            legendGrid.setColor(selectedColor)
        }
    }
}

