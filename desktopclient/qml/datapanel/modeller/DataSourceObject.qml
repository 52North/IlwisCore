import QtQuick 2.0

BasicModellerObject {

    nameText: "DataSource"

    width: 100
    height: 100

    function draw(ctx) {
        console.log(nameText, " drawn")
        ctx.rect(x, y, width, height)
        ctx.stroke()
    }

    function selected(checkX, checkY) {

        return false;
    }
}
