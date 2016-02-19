import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import "../../Global.js" as Global

Item {
    function iconSource(name) {
        if ( name === "")
            name = "redbuttonr.png"
        var iconP = "../../images/" + name
        return iconP

    }
    Text{
        id : label
        text : qsTr("Objects that can be created")
        font.bold: true
    }


    Rectangle{
        anchors.top : label.bottom
        anchors.topMargin: 3
        height : parent.height - label.height - 3
        width : parent.width
        color : Global.alternatecolor2
        border.width: 1
        border.color: Global.edgecolor
        radius : 5

        ScrollView {
            anchors.fill: parent

            ListView {
                id : objectlist
                anchors.fill: parent
                clip : true
                highlight:   Component {
                    Rectangle {
                        width: objectlist.width; height: 23
                        color: Global.selectedColor; radius: 2
                        y: (objectlist && objectlist.currentItem) ? objectlist.currentItem.y : -100
                    }
                }
                onCurrentIndexChanged: {
                    if ( model[currentIndex]){
                        editorList.model.append(objectcreator.creatorInfo(model[currentIndex].split('|')[1]))

                    }
                }
                Component.objectName: {
                    objectlist.currentIndex = -1
                }

                model : objectcreator.createableObjects
                delegate : Component {
                    Item {
                        width: objectlist.width
                        height : 23
                        Row {
                            anchors.fill: parent
                            Item {
                                width : 35
                                height : 23
                                Image {
                                    id : icon
                                    source : iconSource(modelData.split('|')[2])
                                }
                            }
                            Text {
                                text : modelData.split('|')[0]
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                if ( index != objectlist.currentIndex)
                                    objectlist.currentIndex = index
                                else
                                    objectlist.currentIndex = -1
                            }
                        }
                    }
                }
            }
        }
    }
}

