import QtQuick 2.0
import "../../Global.js" as Global

Rectangle {

    function drawArea(ctx, h, w ){
        for(var j =0; j < h; j++){
            var frac = j / h
            var color = representation.color(1.0 - frac)
            ctx.beginPath()
            ctx.strokeStyle = color
            ctx.moveTo(20,j)
            ctx.lineTo(80,j)
            ctx.stroke()
            ctx.closePath()
         }
    }

    function drawBars(ctx, h, w ){
        var step = Math.floor(h / 4)
        for(var j =0; j < h; j = j + step){
            ctx.beginPath()
            var color = representation.name2color("#000000")
            ctx.strokeStyle = color
            ctx.moveTo(20,j)
            ctx.lineTo(85,j)
            ctx.stroke()
            ctx.closePath()
        }
    }

    width: 200
    height: defaultHeight
    Text {
        id : rprName
        width : parent.width - 20
        height : Global.rowHeight
        text : representation.representationName
        font.bold: true
        y : 5
        x : 20
    }

    Canvas{
        anchors.top : rprName.bottom
        width : 100
        height : parent.height - rprName.height
        onPaint : {
            drawArea(getContext("2d"), height - 15, width)
            drawBars(getContext("2d"), height - 15, width)
        }
    }
}
