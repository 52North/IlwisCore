import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0

Item {
    Button {
        id : button
        width : 100
        height : 20
        text : "refresh"
        onClicked: {
            list.model = null
            list.model = uicontext.debugProperty("references")
        }
    }

    ListView{
        id : list
        width : 700
        height : parent.height
        model : uicontext.debugProperty("references")
        anchors.top: button.bottom
        anchors.topMargin: 4
        delegate:Row {
            width : parent.width
            height : 16
            spacing : 2
            Text {
                text : index.toString()
                width : 30
                height : 15
                font.pointSize: 9
                elide: Text.ElideMiddle
            }
            Text {
                text : modelData.name
                width : 110
                height : 15
                font.pointSize: 9
                elide: Text.ElideMiddle
            }
            Text {
                text : modelData.id
                width : 60
                height : 15
                font.pointSize: 9
            }
            Text {
                text : modelData.type
                width : 140
                height : 15
                font.pointSize: 9
                elide: Text.ElideMiddle
            }
            Text {
                text : modelData.referenceCount.toString()
                width : 40
                height : 15
                font.pointSize: 9
            }
        }
    }
}
