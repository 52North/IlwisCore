import QtQuick 2.0

Rectangle {

    function drawBars(ctx, h, w ){
        for(var j =0; j < h; j++){
            var frac = j / h
            var color = representation.color(frac)
            console.debug(color, j, frac)
            ctx.strokeStyle = color
            ctx.beginPath()
            ctx.moveTo(0,j)
            ctx.lineTo(w,j)
            ctx.stroke()
        }
    }

    width: 200
    height: 200
    border.width: 1
    Canvas{
        anchors.fill : parent
        onPaint : {
            drawBars(getContext("2d"), height, width)
        }
    }
}
