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
    property string imagePath
    property string dataSource;

    property AbstractBasicModellerObject parentObject;
    property AbstractBasicModellerObject childObject;


    function hasParentObject() {
        return parentObject != null;
    }

    function hasChildObject() {
        return childObject != null;
    }

     /*
      * Coordinates represent the center of the object
     */
    function setCoordinates(newX, newY) {
        x = newX;
        y = newY;
        if (hasParentObject() && parentObject.nameText !== null && parentObject.nameText === "Connector") {
            parentObject.setEndCoordinate(newX - width/2, newY);
        }
        if (hasChildObject() && childObject.nameText !== null && childObject.nameText === "Connector") {
            childObject.setStartCoordinate(newX + width/2, newY);
        }
    }

    function isSelected(checkX, checkY) {
        selected = containsPosition(checkX, checkY);
        return selected;
    }

    function containsPosition(checkX, checkY) {
        return checkX > (x - width/2) && checkX < (x + width/2) && checkY > (y - height/2) && checkY < (y + height/2);
    }

    function setImagePath(path) {
        if (imagePath === "") {
            imagePath = path;
        }
    }

    function setDataSource(source) {
        if (dataSource === "") {
            dataSource = source;
        }
    }

}
