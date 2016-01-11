import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import StartIlwis 1.0

Window {
    width: 1040
    height: 800
    title: qsTr("Hello World")
    id : rootwindow
    visible: true
    flags: Qt.FramelessWindowHint | Qt.WA_TranslucentBackground
    color: "#00000000"

    function initIlwis() {
        startilwis.init()
        windowloader.source = "DesktopClient.qml"
        startilwis.initMainWindow()
    }

    Loader{
        id : windowloader
        anchors.fill: parent
        source : "Splash.qml"
    }
}

