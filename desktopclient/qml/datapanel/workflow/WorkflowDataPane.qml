import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.1

import "../modeller" as Modeler

Window {

    property bool canSeparate : true

    Modeler.Canvas {
        id: canvas
        anchors.fill: parent

    }
}
