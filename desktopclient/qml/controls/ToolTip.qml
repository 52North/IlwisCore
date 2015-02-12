import QtQuick 2.0
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0

Item {
    id: toolTipRoot
    //width: toolTip.contentWidth
    height: toolTipContainer.height
    visible: false
    clip: false
    z: 999999999
    parent: root

    property alias text: toolTip.text
    property alias radius: content.radius
    property alias backgroundColor: content.color
    property alias textColor: toolTip.color
    property alias font: toolTip.font
    property var target: null

    function onMouseHover(x, y)
    {
        var obj = toolTipRoot.target.mapToItem(toolTipRoot.parent, x, y);
        var obj2 = toolTipRoot.target.mapToItem(root, x, y);
        if ( obj2.x + toolTip.contentWidth > root.width){
            toolTipRoot.x = obj.x - toolTip.contentWidth
            toolTipRoot.y = obj.y + 5;
        }else {
            toolTipRoot.x = obj.x;
            toolTipRoot.y = obj.y + 5;
        }
    }

    function onVisibleStatus(flag)
    {
        if ( flag){
            showTimer.start()
            hideTimer.start()
        }else
            toolTipRoot.visible = flag;
    }

    Component.onCompleted: {
        var itemParent = toolTipRoot.target;

        var newObject = Qt.createQmlObject('import QtQuick 2.0; MouseArea {signal mouserHover(int x, int y);\
signal showChanged(bool flag); anchors.fill:target; hoverEnabled: true; parent:target;propagateComposedEvents:true;\
onPositionChanged: {mouserHover(mouseX, mouseY)} onEntered: {showChanged(true)}\
 onExited:{showChanged(false)} onClicked:{ parent.focus = true; target.onClicked; mouse.accepted = false; } }',
            itemParent, "mouseItem");
        newObject.mouserHover.connect(onMouseHover);
        newObject.showChanged.connect(onVisibleStatus);
    }

    Timer{
        id : showTimer
        interval: 2000
        onTriggered: toolTipRoot.visible = true

    }

    Timer{
        id : hideTimer
        interval: 7000
        onTriggered: toolTipRoot.visible = false;

    }

    Item {
        id: toolTipContainer
        z: toolTipRoot.z + 1
        width: content.width + (2*toolTipShadow.radius)
        height: content.height + (2*toolTipShadow.radius)

        Rectangle {
            id: content
            anchors.centerIn: parent
            width: toolTip.contentWidth + 8
            height: toolTip.contentHeight + 4
            radius: 3
            color : "cornsilk"

            Text {
                id: toolTip
                wrapMode: Text.WordWrap
                x : 4
                anchors.verticalCenter: parent.verticalCenter
            }
        }

    }

    DropShadow {
        id: toolTipShadow
        z: toolTipRoot.z + 1
        anchors.fill: source
        cached: true
        horizontalOffset: 2
        verticalOffset: 2
        radius: 8.0
        samples: 16
        color: "#80000000"
        smooth: true
        source: toolTipContainer
    }

    Behavior on visible { NumberAnimation { duration: 200 }}
}
