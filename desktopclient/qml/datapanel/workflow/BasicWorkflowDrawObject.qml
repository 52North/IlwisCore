import QtQuick 2.0

AbstractBasicWorkflowDrawObject {
    id : basicWorkflowDrawObject

    property string resourceId
    property string nameText
    property string name
    property string title
    property var ctx
    property color borderColor : "black"
    property color color: "#EAECEE"
    property bool selected

    property variant parentObjects : []
    property AbstractBasicWorkflowDrawObject childObject;


    function hasParentObjects() {
        return parentObjects !== null && parentObjects.length > 0;
    }

    function hasChildObject() {
        return childObject != null;
    }

    function addParentObject(parent) {
        var t = parentObjects;
        t.push(parent);
        parentObjects = t;
    }

     /*
      * Coordinates represent the center of the object
     */
    function setCoordinates(newX, newY) {
        x = newX;
        y = newY;
        if (hasParentObjects()) {
            for(var i = 0; i < parentObjects.length; i++) {
                if (parentObjects[i].nameText !== null && parentObjects[i].nameText === "Connector") {
                    parentObjects[i].setEndCoordinate(getInputConnector().x, getInputConnector().y);
                }
            }
        }
        if (hasChildObject() && childObject.nameText !== null && childObject.nameText === "Connector") {
            childObject.setStartCoordinate(getOutputConnector().x, getOutputConnector().y);
        }
    }

    function isSelected(checkX, checkY) {
        selected = containsPosition(checkX, checkY);
        return selected;
    }

    function containsPosition(checkX, checkY) {
        return checkX > (x - width/2) && checkX < (x + width/2) && checkY > (y - height/2) && checkY < (y + height/2);
    }

    function getInputConnector() {
        return Qt.point(x, y);
    }

    function getOutputConnector() {
        return Qt.point(x, y);
    }

    function getName() {
        if (name !== null && name !== "") {
            if (name.indexOf("/") !== -1) {
                var split = name.split("/");
                return split[split.length-1]
            }
            return name
        } else {
            return "not set"
        }
    }

}
