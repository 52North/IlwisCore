import QtQuick 2.1
import RepresentationElement 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Rectangle {
    width : displayEditorColumn.width - 20
    height : 70
    y : 4
     property var editor

    function drawArea(ctx, w, h ){
        ctx.save();
        for(var j =0; j < w; j++){
            ctx.beginPath()
            var frac = j / w
            var color = editor.color(frac)
            ctx.beginPath()
            ctx.lineWidth = 1
            ctx.strokeStyle = color
            ctx.moveTo(j ,20)
            ctx.lineTo(j, 40)
            ctx.stroke()

         }
        ctx.restore()
    }

    function setText(ctx, label, x){
        ctx.lineWidth = 0.4
        var textW = ctx.measureText(label)
        ctx.font = "9px sans-serif normal";
        ctx.text(label, Math.max(0,x - textW.width / 2), 14)
        ctx.stroke()
    }

    function drawBars(ctx, w, h ){
        ctx.save()
        var color = Qt.rgba(0,0,0,1)
        ctx.strokeStyle = color
        var items = editor.representationElements
        if ( items.length === 0)
            return

        var step = Math.floor(w / (items.length - 1))
        var x = 0
        for(var j =0; j < items.length - 1; ++j){
            ctx.beginPath()
            ctx.moveTo(x,20)
            ctx.lineTo(x,45)
            ctx.stroke()
            setText(ctx,items[j].label, x)
            x = x + step
        }
        ctx.beginPath()
        ctx.moveTo(w,20)
        ctx.lineTo(w,45)
        ctx.stroke()
        setText(ctx, items[items.length - 1].label,w)

        ctx.restore()
    }

    function clear(ctx) {
        ctx.reset();
        ctx.clearRect(0, 0, width, height);
        ctx.stroke();
    }

    Controls.TextFieldDropArea {
        function check(id){
            return editor.canUse(id)
        }

        id : rprName
        width : parent.width - 20
        height : Global.rowHeight
        content : editor.representationName
        x : 10
        canUse: check

        onContentChanged: {
            editor.setRepresentation(content)
            rprCanvas.requestPaint()
        }
    }

    Canvas{
        id : rprCanvas
        anchors.top : rprName.bottom
        width : editorColumn1.width - 20
        property bool isDirty : true
        renderTarget:  Canvas.FramebufferObject
        x : 10
        height : 50
        Rectangle{
            x : 0
            y : 19
            width: parent.width - 14
            height : 22
            color : "transparent"
            border.color: "black"
        }

        onPaint : {
            var ctx = getContext("2d")
            drawArea(ctx, width - 15, height)
            drawBars(ctx, width - 15, height)

       }
    }
}
