import QtQuick 2.0

BasicModellerObject {

    nameText: "DataSource"

    function draw(ctx) {
        console.log(nameText, " drawn")
        ctx.rect(x, y,100,100)
        ctx.stroke()
    }
}
