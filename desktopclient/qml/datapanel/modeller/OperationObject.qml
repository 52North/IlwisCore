import QtQuick 2.0

BasicModellerObject {

    nameText: "Operation"

    function draw(ctx) {
        console.log(nameText, " drawn")
        ctx.rect(x, y,50,50)
        ctx.stroke()
    }
}
