import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global

ListView {
    ListModel {
        id: creators
    }
    function oneStepBack(current) {
        if ( editorList.model.count === 1)
            current.state = "invisible"
        if ( editorList.model.count > 1)
            editorList.currentIndex = editorList.model.count - 2
    }

    id : creatorList
    width: parent.width
    model : creators
    orientation : ListView.Horizontal
    highlight: Rectangle{ width : 100; height : creatorList.height; color : Global.selectedColor}
    highlightMoveVelocity : 800

    delegate: Component {

        Rectangle {
            id : mydelegate
            width : creatorList.width
            height : creatorList.height

            function setValue(type, value){
                loader1.item.setValue(type, value)
            }

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

