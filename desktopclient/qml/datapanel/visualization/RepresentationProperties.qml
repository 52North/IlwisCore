import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import RepresentationSetter 1.0
import UIContextModel 1.0
import LayerManager 1.0
import "../../Global.js" as Global

Rectangle {
    width: parent.width
    height: defaultHeight
    property RepresentationSetter representation
    Loader{
        id : rprNumeric

    }
    Loader{
        id : rprThematic
    }

    Component.onCompleted: {
        representation = displayOptions.manager.layer(layerIndex).propertyEditor(editorName)
        if ( representation && representation.activeValueType === "number"){
            rprNumeric.source = "NumericRepresentation.qml"
        }
    }
}
