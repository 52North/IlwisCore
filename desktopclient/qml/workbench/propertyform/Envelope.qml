import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Item {
    id : itemenvelope
    property string envelope
    property bool isLatLon

    Loader {
        anchors.fill: parent
        id : envloader
    }

    Component {
        id : unspefiedenv
        Text { text : envelope
            height : lineheight
            width: parent.width - line3.width - 2

        }
    }
    Component {
        id : bounds
        Controls.CoordinateSystemBounds{
            islatlon: itemenvelope.isLatLon
        }
    }

    Component.onCompleted: {
        var env = envelope
        if ( env === "unspecified"){
            envloader.sourceComponent = unspefiedenv
            itemenvelope.height = Global.rowHeight
        }else {
            envloader.sourceComponent = bounds
            envloader.item.setEnvelope(env);
            envloader.item.readOnly = true
            itemenvelope.height = isLatLon ? Global.rowHeight * 3.5 :  Global.rowHeight * 2.5
        }
    }
}

