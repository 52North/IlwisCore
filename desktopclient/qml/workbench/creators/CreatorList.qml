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

            Loader {
                id : loader1
                anchors.topMargin: 3
                width : parent.width
                source :componentUrl
                onStatusChanged:{
                    if (loader1.status == Loader.Ready) {
                        item.maxHeight = itemHeight
                        item.state = "visible"
                    }
                }
            }
        }
    }

}

