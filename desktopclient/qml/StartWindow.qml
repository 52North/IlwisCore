import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import StartIlwis 1.0

Window {
    width: 543
    height: 401
    title: qsTr("Ilwis 4")
    id : rootwindow
    visible: true
    flags: Qt.FramelessWindowHint | Qt.WA_TranslucentBackground
    color: "transparent"


    function initIlwis() {
        startilwis.init()
        windowloader.source = "DesktopClient.qml"
        startilwis.initMainWindow()
    }

    Loader{
        id : windowloader
        source : "Splash.qml"
    }
}

