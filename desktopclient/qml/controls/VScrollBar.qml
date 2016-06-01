import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick 2.0

Rectangle {
    id : scroller
    property double maxSize : 100
    property double currentSize : 100
    property double currentPosition : -1
    property int oldPosition: -10000
    property int viewportVerticalScrollPosition: -1

    signal scrolled(double position)

    width: 16
    height: parent.height
    border.width: 1
    border.color: "#CBCBCB"
    color : "#E5E5E5"
    enabled : currentSize < maxSize
    opacity : currentSize < maxSize ? 1 : 0

    function calcThumbPostion(){
        var pos = marea.height * scroller.currentPosition / maxSize
        var maxPos = marea.height * (maxSize - currentSize)/maxSize
        if ( pos > maxPos){
            return maxPos
        }
        if ( pos < 0)
            pos = 0
        return pos;
    }

    function scroll(offset){
        var scrollerTopThumbPos =  marea.height * currentPosition / maxSize
        var scrollerBottomThumbPos = marea.height * ( currentPosition + currentSize) / maxSize

        if (viewportVerticalScrollPosition == -1) {
            viewportVerticalScrollPosition = marea.height / 2
        }

        //var viewportMax = marea.height * (maxSize - currentSize)/maxSize;
        var viewportMax = marea.height * maxSize;

        if (scrollerTopThumbPos + offset < 0) // try to keep the scrolling positions limited to upper/bottom positions
            offset =  -scrollerTopThumbPos;

        if (scrollerBottomThumbPos + offset > viewportMax) // try to keep the scrolling positions limited to upper/bottom positions
            offset =  viewportMax - scrollerBottomThumbPos;

        if ( (scrollerTopThumbPos + offset) >= 0 && (scrollerBottomThumbPos + offset) <= viewportMax){
            viewportVerticalScrollPosition += offset
            if ( oldPosition == -10000){
                oldPosition = viewportVerticalScrollPosition // keep for the scroller draggable button
            }

            var rely = viewportVerticalScrollPosition / marea.height
            var oldRely = oldPosition / marea.height
            var difrely = rely - oldRely;
            currentPosition = currentPosition + maxSize * difrely
            if ( currentPosition > maxSize - currentSize)
                currentPosition = maxSize - currentSize
            if ( (currentPosition) < 0)
                currentPosition = 0
            scrolled(currentPosition)

            oldPosition = viewportVerticalScrollPosition
        }
    }



    Button{
        id : topMarker
        height : 14
        width : parent.width - 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top : parent.top
        Image {
            source : "../images/darkbluearrow.png"
            width : 6
            height : 8
            anchors.centerIn: parent
            rotation: -180
        }
        onClicked: scroll(-10)
    }
    Button{
        id : bottomMarker
        height : 14
        width : parent.width - 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom : parent.bottom
        anchors.bottomMargin: parent.width
        Image {
            source : "../images/darkbluearrow.png"
            width : 6
            height : 8
            anchors.centerIn: parent
            rotation: 0
        }
        onClicked: scroll(10)
    }

    Button {
        id : scrollerThumb
        height : Math.max(10,parent.height * currentSize / maxSize)
        width : parent.width - 2
        y : topMarker.height + calcThumbPostion()
        anchors.horizontalCenter: parent.horizontalCenter
    }
    MouseArea {
        id :marea
        anchors.top : topMarker.bottom
        anchors.bottom: bottomMarker.top
        width : parent.width
        hoverEnabled: false

        onPositionChanged: {
            if ( pressed && containsMouse){
                var scrollerTopThumbPos =  marea.height * currentPosition / maxSize
                var scrollerBottomThumbPos = marea.height * ( currentPosition + currentSize) / maxSize

                if ( mouse.y > scrollerTopThumbPos && mouse.y < scrollerBottomThumbPos){
                    viewportVerticalScrollPosition = mouse.y;
                    if ( oldPosition == -10000){
                        oldPosition = viewportVerticalScrollPosition
                    }

                    var rely = viewportVerticalScrollPosition / marea.height
                    var oldRely = oldPosition / marea.height
                    var difrely = rely - oldRely;
                    currentPosition = currentPosition + maxSize * difrely
                    if ( currentPosition > maxSize - currentSize)
                        currentPosition = maxSize - currentSize
                    if ( (currentPosition) < 0)
                        currentPosition = 0
                    scrolled(currentPosition)

                    oldPosition = viewportVerticalScrollPosition
                }
            }
        }
    }
}

