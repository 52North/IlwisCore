import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

Button {
    id : but
    height : 25
    width : 25
    property alias iconSource : image.source
    property alias tooltip : ttip.text

    Rectangle {
        anchors.centerIn: parent
        height : 23
        width : 23
        color : but.checked ? "lightgrey" : "transparent"


        Image {
            id : image
            height : 20
            width : 20
            anchors.centerIn: parent

        }

    }
    ToolTip{
        id : ttip
        target : parent
    }
}
