import QtQuick 2.0

Image {
    id: img
    anchors.fill: parent
    anchors.margins: 2
    property string imageName

    source: iconSource(imageName)
    fillMode: Image.PreserveAspectFit
    asynchronous: true
}

