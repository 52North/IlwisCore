import QtQuick 2.0



Rectangle {
    id:comboBox
    property variant items: ["a","b","c","de", "f","ff"];
    property alias selectedItem: chosenItemText.text;
    property alias selectedIndex: listView.currentIndex;
    property Component dropDelegate : defaultDelegate
    signal comboClicked;
    //            width: 100;
    //            height: 30;
    z: 100;
    smooth:true;
    color : "transparent"

    Rectangle {
        id:chosenItem
        radius:2;
        width:parent.width;
        height:comboBox.height;
        color: "#eee"
        border.width: 1
        border.color: "#9494B8"
        smooth:true;
        Text {
            width: parent.width-50; height: parent.height
            anchors.left: parent.left; anchors.leftMargin: 10;
            verticalAlignment: Text.AlignVCenter
            id:chosenItemText
            text:comboBox.items[0];
            font.family: "Arial"
            font.pixelSize: 12;
            smooth:true
        }

        Item {
            width: 20; height: 20
            anchors.right: parent.right
            Image {
                width: 16; height:16;
                anchors.centerIn: parent;
                source: "../images/down20.png"
                fillMode: Image.PreserveAspectFit
            }
        }

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                comboBox.state = comboBox.state==="dropDown"?"":"dropDown"
            }
        }
    }

    Component{
        id : defaultDelegate
        Item{
            width:comboBox.width;
            height: comboBox.height;

            Text {
                text: modelData
                z : 1
                anchors.top: parent.top; anchors.topMargin: 5
                anchors.left: parent.left; anchors.leftMargin: 10
                font.pixelSize: 12
                //                            anchors.margins: 5;

            }
            Rectangle {
                id : selection
                width:comboBox.width;
                height:comboBox.height;
                color: "green"; radius: 5
                opacity : 0
            }
            MouseArea {
                anchors.fill: parent;
                hoverEnabled: true
                onClicked: {
                    comboBox.state = ""
                    var prevSelection = chosenItemText.text
                    chosenItemText.text = modelData
                    if(chosenItemText.text != prevSelection){
                        comboBox.comboClicked();
                    }
                    listView.currentIndex = index;
                }
                onEntered: {
                    selection.opacity = 0.2
                }
                onExited:
                    selection.opacity = 0
            }
        }
    }

    Rectangle {
        id:dropDown
        width:comboBox.width;
        height:0;
        clip:true;
        radius:4;
        anchors.top: chosenItem.bottom;
        anchors.margins: 1;
        color: "#eee"
        border.width: 1
        border.color: "#B0B0B0"

        ListView {
            id:listView
            height:20*comboBox.items.length;
            model: comboBox.items
            currentIndex: 0
            highlight: highlightBar
            highlightFollowsCurrentItem: false
            delegate: dropDelegate
        }
    }

    Component {
        id: highlightBar
        Rectangle {
            width:comboBox.width;
            height:comboBox.height;
            color: "lightsteelblue"; radius: 5
            y: listView.currentItem.y
            Behavior on y {
                SpringAnimation {
                    spring: 3
                    damping: 0.2
                }
            }
        }
    }


    states: State {
        name: "dropDown";
        PropertyChanges { target: dropDown; height:22*comboBox.items.length }
    }

    transitions: Transition {
        NumberAnimation { target: dropDown; properties: "height"; easing.type: Easing.OutExpo; duration: 400 }
    }
}
