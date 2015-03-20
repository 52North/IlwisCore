import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.1

Window {

    x : 100
    y : 100

    property bool canSeparate : true

    Canvas {
        id: canvas
        anchors.fill: parent

    }
}
