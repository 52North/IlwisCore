import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

Button {
    height : 25
    width : 25
    property alias iconSource : image.source
    property alias tooltip : ttip.text

    Image {
        id : image
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

    }
    ToolTip{
        id : ttip
        target : parent
    }
}
