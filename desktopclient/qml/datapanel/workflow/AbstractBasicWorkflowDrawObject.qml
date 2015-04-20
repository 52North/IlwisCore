import QtQuick 2.0


/*
 * Abstract basic object for Workflow that can be drawn
 */
Item {
        id : abstractBasicWorkflowDrawObject

       /*
        * Coordinates represent the center of the object
        */
       function setCoordinates(newX, newY) {
           x = newX;
           y = newY;
       }

       /*
        * Get connection point for Inputs
        */
       function getInputConnector() {
           return Qt.point(x, y);
       }

       /*
        * Get connection point for Outputs
        */
       function getOutputConnector() {
           return Qt.point(x, y);
       }
}
