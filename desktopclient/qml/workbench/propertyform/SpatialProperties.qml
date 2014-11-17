import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1

Rectangle {
    CoordinateSystemProperties{
        id : csyProps
    }
    Loader {
        id : rasterProps
        width : parent.width
        source : typeName == "rastercoverage" ? "GeoreferenceProperties.qml" : ""
        anchors.top : csyProps.bottom
    }
}
