import QtQuick 2.0

Rectangle {
    property int imageHeight : 15
    property var func
    width: 1
    height: 10
    Image {
        id : image
        y : parent.height / 2
        x : -imageHeight / 2
        height : imageHeight
        width : imageHeight
        source : "../images/splithandle.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if ( func){
                    func(styleData.index,mouseX >= width/2 ? 1 : 0)
                }
            }
        }

    }

    color: "lightblue"
}

