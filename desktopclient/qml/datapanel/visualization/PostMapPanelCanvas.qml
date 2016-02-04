import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls


Canvas {
    id : postCanvas
    property var  drawers : []


    function addDrawer(url, parameters){
        var component = Qt.createComponent(url);
        if (component.status === Component.Ready) {
            var drawer = component.createObject(postCanvas, parameters);
            drawers.push(drawer)
         }
    }

    onPaint: {
        var ctx = postCanvas.getContext('2d')
        ctx.save()
        for(var i = 0; i < drawers.length; ++i){
            if (typeof drawers[i].draw == 'function')
                drawers[i].draw(ctx)
        }

        ctx.restore()

    }
}

