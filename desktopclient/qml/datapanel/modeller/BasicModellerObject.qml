import QtQuick 2.0
import QtQuick.Controls 1.0
import "modeller.js" as Code

AbstractBasicModellerObject {
    id : basicModellerObject

    property string nameText
    property var ctx
    property color borderColor : "black"
    property color color: "#EAECEE"
    property bool selected

    property AbstractBasicModellerObject parentObject;
    property AbstractBasicModellerObject childObject;

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

    function hasParentObject() {
        return parentObject != null;
    }

    function hasChildObjtect() {
        return childObject != null;
    }
}
