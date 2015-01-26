var itemComponent = null;
var draggedItem = null;
var startingMouse;
var posnInWindow;

function startDrag(mouse)
{
    posnInWindow = paletteItem.mapToItem(window, 0, 0);
    startingMouse = { x: mouse.x, y: mouse.y }
    loadComponent();
}

//Creation is split into two functions due to an asynchronous wait while
//possible external files are loaded.

function loadComponent() {
    if (itemComponent != null) { // component has been previously loaded
        createItem();
        return;
    }

    itemComponent = Qt.createComponent(paletteItem.componentFile);
    if (itemComponent.status == Component.Loading)  //Depending on the content, it can be ready or error immediately
        component.statusChanged.connect(createItem);
    else
        createItem();
}

function createItem() {
    if (itemComponent.status == Component.Ready && draggedItem == null) {
        draggedItem = itemComponent.createObject(item2, {"image": paletteItem.image, "x": posnInWindow.x - item2.x, "y": posnInWindow.y - item2.y, "z": 3});
        // make sure created item is above the ground layer
    } else if (itemComponent.status == Component.Error) {
        draggedItem = null;
        console.log("error creating component");
        console.log(itemComponent.errorString());
    }
}

function continueDrag(mouse)
{
    if (draggedItem == null)
        return;
      draggedItem.x = mouse.x - item2.x;
      draggedItem.y = mouse.y - item2.y;

    // show only in item2 window
    if (mouse.x > item2.x
            && mouse.x  < item2.x+item2.width
            && mouse.y  > item2.y
            && mouse.y  < item2.y+item2.height) {
        draggedItem.opacity = 1;
    } else {
        draggedItem.opacity = 0;
    }
}

function endDrag(mouse)
{
    if (draggedItem == null)
        return;

    if (mouse.x < item2.x
            || mouse.x  > item2.x+item2.width
            || mouse.y  < item2.y
            || mouse.y  > item2.y+item2.height) {
        draggedItem.destroy();
        draggedItem = null;
    } else {
        draggedItem.created = true;
        draggedItem = null;
    }
}

function startMoveItem(mouse, item) {
    if (item === null)
        return;

    //if (item.objectName != null && item.objectName == "test") {
        draggedItem = item;
//        posnInWindow = item.mapToItem(item2, 0, 0);
    //}
}

function continueMoveItem(mouse)
{
    if (draggedItem == null)
        return;

    if (mouse.buttons === Qt.LeftButton) {
        draggedItem.x = draggedItem.x + mouse.x - draggedItem.width/2;
        draggedItem.y = draggedItem.y + mouse.y - draggedItem.height/2;
    }

//    if (draggedItem.x > 0 - draggedItem.width/2
//            && draggedItem.x  < item2.width-draggedItem.width/2
//            && draggedItem.y  > 0 - draggedItem.height/2
//            && draggedItem.y  < item2.height-draggedItem.height/2) {
//        draggedItem.opacity = 0.5;
//    } else {
//        draggedItem.opacity = 0;
//    }
}

function endMoveItem(mouse)
{
    if (draggedItem == null)
        return;


    if (draggedItem.x < 0) {
        draggedItem.x = 0
    }
    if (draggedItem.x + draggedItem.width > centerItem.width) {
        draggedItem.x = centerItem.width - draggedItem.width
    }
    if (draggedItem.y < 0) {
        draggedItem.y = 0
    }
    if (draggedItem.y + draggedItem.height*2 > centerItem.height) {
        draggedItem.y = centerItem.height - draggedItem.height - statusB.height - tabtools.height - toolbar.height
    }
    draggedItem.opacity = 1;
    draggedItem = null;
}

