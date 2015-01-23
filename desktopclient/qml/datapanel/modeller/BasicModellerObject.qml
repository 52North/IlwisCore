import QtQuick 2.0
import QtQuick.Controls 1.0
import "modeller.js" as Code

Item {
    id : basicModellerObject
    state: "inactive"

    property string nameText
    property var ctx
    property color borderColor : "black"
    property color color: "#EAECEE"
    property bool selected

     /*
      * Coordinates represent the center of the object
     */
    function setCoordinates(newX, newY) {
        x = newX;
        y = newY;
    }

    function isSelected(checkX, checkY) {
        selected = containsPosition(checkX, checkY);
        return selected;
    }
    function containsPosition(checkX, checkY) {
        return checkX > (x - width/2) && checkX < (x + width/2) && checkY > (y - height/2) && checkY < (y + height/2);
    }

    Text {
        id: name
        text: qsTr(nameText)
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        rotation: -parent.rotation
    }

    MouseArea {
        id: mousearea
        anchors.fill: parent
        hoverEnabled:true
        onPressed: {
            if (basicModellerObject.state === "active") {
            } else {
                Code.startMoveItem(mouse, parent);
            }

        }
        onPositionChanged:  {
            if (basicModellerObject.state === "active") {

            } else {
                Code.continueMoveItem(mouse);
            }
        }
        onReleased: {
            if (basicModellerObject.state === "active") {

            } else {
                Code.endMoveItem(mouse);
            }

        }
        onEntered: parent.opacity = 0.5;
        onExited:  parent.opacity = 1;
        onClicked: {
//            if (mouse.button === Qt.RightButton) {
//                if (basicModellerObject.state === "active") {
//                    basicModellerObject.state = "inactive"
//                } else {
//                    basicModellerObject.state = "active"
//                }
//            }
        }
    }

    states: [
        State {
            name: "active"
            PropertyChanges {
                target: basicModellerObject
                color: "lightblue"
            }
        },
        State {
            name : "inactive"
            PropertyChanges {
                target: basicModellerObject
                color: "#EAECEE"
            }
        }
    ]
}
