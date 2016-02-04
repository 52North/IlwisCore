import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../../Global.js" as Global


Rectangle {
    property int standardHeight: 180
    property int standardWidth: 200
    property int padding: 20

    id : conditionItem
    width: standardWidth
    height: standardHeight
    color: "#80bfff"
    border.width: 1
    border.color: "black"
    transformOrigin: Item.TopLeft;
    z: 0
    transform: Translate { id: transformTl }
    property var operationsList : []
    property var conditionContainerCanvas
    property var canvasComponent
    property var containerId

    function panOperation(x, y)
    {
        transformTl.x += x;
        transformTl.y += y;
    }

    function getXYcoordsCanvas()
    {
        var pt = {x: 0, y: 0};
        pt.x = conditionItem.x + transformTl.x;
        pt.y = conditionItem.y + transformTl.y;
        return pt;
    }

    function replacePanOperation(x, y)
    {
        transformTl.x = x - conditionItem.x ;
        transformTl.y = y - conditionItem.y ;
    }

    function scaleOperation(scaleFactor)
    {
        conditionItem.scale *= scaleFactor;
    }

    function transformedPoint(matrix, x, y)
    {
        return {
            x: matrix.inverse().a * x + matrix.inverse().c * y + matrix.inverse().e,
            y: matrix.inverse().b * x + matrix.inverse().d * y + matrix.inverse().f
        }
    }

    function getScreenCoords(matrix, x, y) {
        var xn = matrix.e + x * matrix.a;
        var yn = matrix.f + y * matrix.d;
        return { x: xn, y: yn };
    }

    function setCanvasColor(color){
        conditionRectangle.color = color;
    }

    function addToOperationList(operationIndex) {
        if(operationsList.indexOf(operationIndex) == -1) {
            operationsList.push(operationIndex);
            resize()
        }
    }

    function removeFromOperationList(operationIndex) {
        if(operationsList.indexOf(operationIndex) > -1) {
            operationsList.splice(operationsList.indexOf(operationIndex), 1)
            resize()
        }
    }

    function finishCreation(component) {
        if (canvasComponent.status == Component.Ready) {
            var sprite = canvasComponent.createObject(conditionRectangle, {});
            if (sprite == null) {
                console.log("Error creating object");
            }
        } else if (canvasComponent.status == Component.Error) {
            console.log("Error loading component:", canvasComponent.errorString());
        }
    }

    function resize() {
        var minX = 1000000, maxX = -1000000, minY = 1000000, maxY = -1000000, operation, operationIndex, xChanged = false, yChanged = false;

        // Search for min and max, x and y
        for (var i in operationsList) {
            operationIndex = operationsList[i]
            operation = wfCanvas.operationsList[operationIndex]

            if (operation.x < minX) {
                minX = operation.x
                xChanged = true
            }
            if (operation.x + operation.width > maxX) {
                maxX = operation.x + operation.width
                xChanged = true
            }

            if (operation.y < minY) {
                minY = operation.y;
                yChanged = true
            }
            if (operation.y + operation.height > maxY) {
                maxY = operation.y + operation.height
                yChanged = true
            }
        }

        conditionItem.width = xChanged ? maxX - minX + (padding * 2) : standardWidth
        conditionItem.height = yChanged ? maxY - minY + listRectangle.height + (padding * 2) : standardHeight
        if (xChanged) conditionItem.x = minX - padding
        if (yChanged) conditionItem.y = minY - listRectangle.height - padding
    }

    function openNewConditionDialogButtonFunction() {
        wfCanvas.showConditionTypeForm(containerId)
    }

    function addCondition(conditionId, name) {
        testModel.append({
            'first': false,
            'condition': '',
            'second': false,
            'xId': conditionId,
            'name': name
        })
    }

    function refresh() {
        var conditions = wfCanvas.getConditions(containerId)
        testModel.clear()

        for (var i = 0; i < conditions.length; i++){
            testModel.append(conditions[i])
        }
    }

    ListModel {
        id: testModel
    }

    Rectangle {
        id: listRectangle

        border.width: 1
        border.color: "black"
        color: "#cce5ff"
        height : 65
        width : parent.width - 8
        x : 4
        y: 4

        ListView{
            id : conditionList

            clip : true
            height : parent.height - 25
            width : parent.width
            x: 0
            y: 0

            model : testModel
            delegate: Row {
                Column {
                    Loader{
                        width: conditionList.width - 30
                        height: 15
                        source: name !== "" ? "OnlyNameCondition.qml" : "StandardCondition.qml"
                    }
                }
                Column {
                    Button {
                        height : 15
                        width : 15
                        Image {
                            anchors.fill: parent
                            source : "../../../images/refresh20.png"
                        }
                        onClicked: {
                            var info = canvas.workflow.getOpenConditionParameters(containerId, xId)
                            manager.showConditionForm(info.operationId, info.hiddenFields, info.constantValues, containerId + '|' + xId)
                        }
                    }
                }
                Column {
                    Button {
                        height : 15
                        width : 15
                        Image {
                            anchors.fill: parent
                            source : "../../../images/close20.png"
                        }
                        onClicked: {
                            // TODO: remove condition
                            refresh()
                        }
                    }
                }
            }
        }
        Button {
            height : 25
            width : 25
            anchors.top : conditionList.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            Image {
                anchors.fill: parent
                source : "../../../images/createCS1.png"
            }
            onClicked: {
                openNewConditionDialogButtonFunction()
            }
        }
    }


    Rectangle {
        id: conditionRectangle

        anchors.topMargin: 4
        anchors.top: listRectangle.bottom
        border.width: 1
        border.color: "black"
        color : "white"
        height: parent.height - listRectangle.height - 12
        width : parent.width - 8
        x : 4
    }
}
