import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.1

import "../modeller" as Modeler

Item {

    property bool canSeparate : true

    // TODO react on signals, e.g. when edit session is cancelled

    Modeler.ModellerPanel {
        id: modelerPanel
        anchors.fill: parent

    }
}
