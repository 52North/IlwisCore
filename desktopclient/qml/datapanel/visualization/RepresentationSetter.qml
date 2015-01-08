import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global

Rectangle {
    width: parent.width
    height: 62
    ComboBox{
        width : parent.width
        height : Global.rowHeight
        model : ["red","blue","green"]
    }
}
