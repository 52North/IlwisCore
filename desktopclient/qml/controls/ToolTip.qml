import QtQuick 2.0
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0

MouseArea {
    id: tooltipArea
    anchors.fill : parent
    clip: false
    z: 999999999
    hoverEnabled: true
    propagateComposedEvents:true


    property string text
    property int radius : 2
    property var backgroundColor : "#E6FAE6"
    property var textColor
    property var font
    property var target
    property bool active : false
    property bool up : false

    function stopTimer(){
        active = false;
        showTimer.running = false
        hideTimer.running = false;

    }

    onEntered: {
        onVisibleStatus(true)
        active = true
        changeToolTipPosition(mouseX, mouseY)
    }

    onExited: {
       onVisibleStatus(false)
       active = false
    }

    onPositionChanged: {
        mouse.accepted = false
        changeToolTipPosition(mouseX, mouseY)
    }

    onClicked: {
        mouse.accepted = false
        if ( typeof target.mouseClicked === "function")
            target.mouseClicked()
    }

    onPressed: mouse.accepted = false;
    onReleased: mouse.accepted = false;
    onDoubleClicked: mouse.accepted = false;
    onPressAndHold: mouse.accepted = false;

    function changeToolTipPosition(x,y) {
        if (active && target && text != ""){
            var obj = mapToItem(root, x, y);
            if ( obj){
                toolTip.x = obj.x + 5
                toolTip.y = obj.y + (up ? -23 : 5)
            }
        }
    }

    function onVisibleStatus(flag)
    {
        if ( text == "")
            return;

        if ( target){
            if ( flag){
                showTimer.start()
                hideTimer.start()
                toolTip.text = text
                toolTip.radius = radius
                toolTip.backgroundColor = backgroundColor
            }else{
                showTimer.stop()
                hideTimer.stop()
                toolTip.visible = flag;
            }
        }
    }

    Timer{
        id : showTimer
        interval: 1500
        onTriggered: {
            toolTip.visible = true


        }

    }

    Timer{
        id : hideTimer
        interval: 12000
        onTriggered: {
            toolTip.visible = false
        }

    }


}
