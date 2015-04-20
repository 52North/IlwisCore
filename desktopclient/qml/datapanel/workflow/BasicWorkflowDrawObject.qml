import QtQuick 2.0

/*
 * Basic object for Workflow that can be drawn with further functionality
 */
AbstractBasicWorkflowDrawObject {
    id : basicWorkflowDrawObject

    property string resourceId
    property string nameText
    property string name
    property string title
    // Context2D toi draw this object
    property var ctx
    property color borderColor : "black"
    property color color: "#EAECEE"
    // Indocator if selected
    property bool selected
    // multiple parent objects
    property variant parentObjects : []
    property AbstractBasicWorkflowDrawObject childObject;

    /*
     * Has parent objects
     */
    function hasParentObjects() {
        return parentObjects !== null && parentObjects.length > 0;
    }

    /*
     * Has child object
     */
    function hasChildObject() {
        return childObject != null;
    }

    /*
     * Add a parent object
     */
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

    /*
     * Check if the object is selected
     */
    function isSelected(checkX, checkY) {
        selected = containsPosition(checkX, checkY);
        return selected;
    }

    /*
     * Check if the coordinates are inside the objecs borders/buffer
     */
    function containsPosition(checkX, checkY) {
        return checkX > (x - width/2) && checkX < (x + width/2) && checkY > (y - height/2) && checkY < (y + height/2);
    }

    /*
     * Get the object name or 'not set'
     */
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
