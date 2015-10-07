import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0

Item {
    id : csyBoundingBox
    property string minx : ""
    property string miny : ""
    property string maxx : ""
    property string maxy : ""


    Component{
        id : metric
        Item {
            anchors.fill: parent

            Column {
                anchors.fill: parent
                spacing : 4
                Row {
                    width : parent.width
                    height : 20
                    spacing : 2
                    Text {
                        text : qsTr("MinX/MinY")
                        width : 120
                        height : 20
                        font.bold: true
                    }
                    TextField{id : minxfld
                        width : 110
                        height : 20
                        text : minx
                        onTextChanged: {
                            minx = text
                        }
                    }
                    TextField{
                        id : minyfld
                        width : 110
                        height : 20
                        text : miny
                        onTextChanged: {
                            miny = text
                        }
                    }
                }
                Row {
                    width : parent.width
                    height : 20
                    spacing : 2
                    Text {
                        text : qsTr("MaxX/MaxY")
                        font.bold: true
                        width : 120
                        height : 20
                    }
                    TextField{
                        id : maxxfld
                        width : 110
                        height : 20
                        text : maxx
                        onTextChanged: {
                            maxx = text
                        }
                    }
                    TextField{
                        id : maxyfld
                        width : 110
                        height : 20
                        text : maxy
                        onTextChanged: {
                            maxy = text
                        }
                    }
                }
            }
        }
    }
    Loader {
        id : valuepairs
        width : parent.width
        height : 40
        sourceComponent : metric
    }
}

