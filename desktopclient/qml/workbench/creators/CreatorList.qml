import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global

ListView {
    id : creatorList
    width: parent.width
    model : objectcreator.activeCreators
    orientation : ListView.Horizontal
    delegate: Component {
        Rectangle {
            width : creatorList.width
            height : creatorList.height


            Rectangle{
                id : labelrect
                width : parent.width
                height : 22
                color : Global.headerdark
                Text{
                    text : label
                    color: "white"
                    x : 4
                    font.pointSize: 9
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                }

            }
            Loader {
                anchors.top : labelrect.bottom
                anchors.topMargin: 3
                width : parent.width
                source : componentUrl
            }

        }
    }
}

