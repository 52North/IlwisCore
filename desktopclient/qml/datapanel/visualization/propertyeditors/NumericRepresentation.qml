import QtQuick 2.1
import "../../../Global.js" as Global

Rectangle {
    width : displayEditorColumn.width - 20
    height : 70
    y : 4
     property var editor

    function drawArea(ctx, w, h ){
        ctx.save();
        for(var j =0; j < w; j++){
            var frac = j / w
            var color = editor.color(1.0 - frac)
            ctx.beginPath()
            ctx.strokeStyle = color
            ctx.moveTo(j ,20)
            ctx.lineTo(j, 40)
            ctx.stroke()

         }
        ctx.restore()
    }

    function drawBars(ctx, w, h ){
        ctx.save()
        var color = Qt.rgba(0,0,0,1)
        ctx.strokeStyle = color
        var step = Math.floor(w / 4)
        for(var j =0; j < w; j = j + step){
            ctx.beginPath()
            ctx.moveTo(j,20)
            ctx.lineTo(j,45)
            ctx.stroke()
            ctx.beginPath()
            ctx.text("100", j, 10)
            ctx.stroke()
        }
        ctx.beginPath()
        ctx.moveTo(w,20)
        ctx.lineTo(w,45)
        ctx.stroke()

        ctx.restore()
    }

    Text {
        id : rprName
        width : parent.width - 20
        height : Global.rowHeight
        text : editor.representationName
        font.bold: true
        x : 2
    }

    Canvas{
        id : rprCanvas
        anchors.top : rprName.bottom
        width : editorColumn1.width - 20
        x : 20
        height : 50
        onPaint : {
           // drawArea(getContext("2d"), width - 15, height)
            drawBars(getContext("2d"), width - 15, height)

        }
    }
}
