import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import RepresentationSetter 1.0
import UIContextModel 1.0
import VisualizationManager 1.0
import "../../Global.js" as Global

Rectangle {
    width: parent.width
    height: 62
    property RepresentationSetter representation
    ComboBox{
        width : parent.width
        height : Global.rowHeight
        Component.onCompleted: {
             model : representation.rprNames
        }
    }

    Component.onCompleted: {
        representation = new RepresentationSetter(parent)
        representation.prepare()
    }
}
