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
    property int viewportHorizontalScrollPosition: -1

    signal scrolled(double position)

    width: parent.width
    height: 16
    border.width: 1
    border.color: "#CBCBCB"
    color : "#E5E5E5"
    enabled : currentSize < maxSize
    opacity : currentSize < maxSize ? 1 : 0

    function calcThumbPostion(){
        var pos = marea.width * scroller.currentPosition / maxSize
        var maxPos = marea.width * (maxSize - currentSize)/maxSize
        if ( pos > maxPos){
            return maxPos
        }
        if ( pos < 0)
            pos = 0
        return pos;
    }

    function scroll(offset){
        var scrollerLeftThumbPos =  marea.width * currentPosition / maxSize
        var scrollerRightThumbPos = marea.width * ( currentPosition + currentSize) / maxSize

        if (viewportHorizontalScrollPosition == -1) {
            viewportHorizontalScrollPosition = marea.width / 2
        }

        //var viewportMax = marea.height * (maxSize - currentSize)/maxSize;
        var viewportMax = marea.width * maxSize;

        if (scrollerLeftThumbPos + offset < 0) // try to keep the scrolling positions limited to upper/bottom positions
            offset =  -scrollerLeftThumbPos;

        if (scrollerRightThumbPos + offset > viewportMax) // try to keep the scrolling positions limited to upper/bottom positions
            offset =  viewportMax - scrollerRightThumbPos;

        if ( (scrollerLeftThumbPos + offset) >= 0 && (scrollerRightThumbPos + offset) <= viewportMax){
            viewportHorizontalScrollPosition += offset
            if ( oldPosition == -10000){
                oldPosition = viewportHorizontalScrollPosition // keep for the scroller draggable button
            }

            var relx = viewportHorizontalScrollPosition / marea.width
            var oldRelx = oldPosition / marea.width
            var difrelx = relx - oldRelx;
            currentPosition = currentPosition + maxSize * difrelx
            if ( currentPosition > maxSize - currentSize)
                currentPosition = maxSize - currentSize
            if ( (currentPosition) < 0)
                currentPosition = 0
            scrolled(currentPosition)

            oldPosition = viewportHorizontalScrollPosition
        }
    }

    Button{
        id : leftMarker
        height : parent.height - 2
        width : 14
        anchors.verticalCenter: parent.verticalCenter
        anchors.left : parent.left
        Image {
            source : "../images/darkbluearrow.png"
            width : 8
            height : 6
            anchors.centerIn: parent
            rotation: 90
        }
        onClicked: scroll(-10)
    }
    Button{
        id : rightMarker
        height : parent.height - 2
        width : 14
        anchors.verticalCenter: parent.verticalCenter
        anchors.right : parent.right
        anchors.rightMargin: parent.height
        Image {
            source : "../images/darkbluearrow.png"
            width : 8
            height : 6
            anchors.centerIn: parent
            rotation: -90
        }
        onClicked: scroll(10)
    }

    Button {
        id : scrollerThumb
        width : Math.max(10,parent.width * currentSize / maxSize)
        height : parent.height - 2
        x : leftMarker.width + calcThumbPostion()
        anchors.verticalCenter: parent.verticalCenter
    }
    MouseArea {
        id :marea
        anchors.left : leftMarker.right
        anchors.right: rightMarker.left
        height : parent.height
        hoverEnabled: false


        onPositionChanged: {
            if ( pressed && containsMouse){
                var scrollerLeftThumbPos =  marea.width * currentPosition / maxSize
                var scrollerRightThumbPos = marea.width * ( currentPosition + currentSize) / maxSize
                if ( mouse.x > scrollerLeftThumbPos && mouse.x < scrollerRightThumbPos){
                    viewportHorizontalScrollPosition = mouse.x;
                    if ( oldPosition == -10000){
                        oldPosition = viewportHorizontalScrollPosition
                    }

                    //if ( oldPosition != -10000){
                        var relx = viewportHorizontalScrollPosition / marea.width
                        var oldRelx = oldPosition / marea.width
                        var difrelx = relx - oldRelx;
                        currentPosition = currentPosition + maxSize * difrelx
                        if ( currentPosition > maxSize - currentSize)
                            currentPosition = maxSize - currentSize
                        if ( (currentPosition) < 0)
                            currentPosition = 0
                        scrolled(currentPosition)
                    //}
                    oldPosition = viewportHorizontalScrollPosition

                }
            }
        }
    }   
}

