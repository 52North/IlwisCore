import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls
import LayersView 1.0

Item {

    Controls.HScrollBar{
        anchors.bottom : parent.bottom
        id :hscroller
        objectName: uicontext.uniqueName()

        function updateItem(){
            var envView = maparea.drawer.viewEnvelope
            maxSize = envView.maxx - envView.minx
            var envZoom = maparea.drawer.zoomEnvelope
            oldPosition = -10000
            currentSize = envZoom.maxx - envZoom.minx
            if ( currentPosition === -1)
                currentPosition = 0
            else{
                currentPosition = envZoom.minx - envView.minx

            }

            return 0
        }

        onScrolled: {
            var envZoom = maparea.drawer.zoomEnvelope
            var envView = maparea.drawer.viewEnvelope
            var newPos1 = envView.minx + position
            var newPos2 = newPos1 + envZoom.maxx - envZoom.minx
            envZoom.minx = newPos1
            envZoom.maxx = newPos2
            maparea.drawer.zoomEnvelope = envZoom

        }
    }
    Controls.VScrollBar{
        anchors.right : parent.right
        id :vscroller
        objectName: uicontext.uniqueName()

        function updateItem(){
            var envView = maparea.drawer.viewEnvelope
            maxSize = envView.maxy - envView.miny
            var envZoom = maparea.drawer.zoomEnvelope
            currentSize = envZoom.maxy - envZoom.miny
            oldPosition = -10000
            if ( currentPosition === -1)
                currentPosition = 0
            else{
                currentPosition = envView.maxy - envZoom.maxy
            }

            return 0
        }

        onScrolled: {
            var envZoom = maparea.drawer.zoomEnvelope
            var envView = maparea.drawer.viewEnvelope
            var newPos1 = envView.maxy - position
            var newPos2 = newPos1 - (envZoom.maxy - envZoom.miny)
            envZoom.miny = newPos2
            envZoom.maxy = newPos1
            maparea.drawer.zoomEnvelope = envZoom

        }
    }
    Component.onCompleted: {
        drawer().associate(hscroller.objectName,"synchronizeEnded")
        drawer().associate(vscroller.objectName,"synchronizeEnded")
    }
}

