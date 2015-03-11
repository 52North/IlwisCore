import QtQuick 2.2;
import QtQuick.Controls 1.1;
import QtQuick.Layouts 1.1;

Column {
    property string formresult : "safasdf";
    property string outputfield_0;
    x:5;
    width : parent.width - 5;
    height : parent.height;
    spacing : 10;
//    ExclusiveGroup {
//        id : sourceFilterGroup;
//        onCurrentChanged: {}
//    }

    function getName() {
        return modelNameText.text
    }

    function getDescription() {
        return modelDescriptionText.text
    }

    Rectangle {
        id: nameItem
        x : 5
        y : 5
        height : 22
        width : parent.width - 5;
        color : background1;
        Text {
            id : modelNameLabel
            height : 20
            text : qsTr("Name");
            width : modelDescriptionLabel.width
            wrapMode:Text.Wrap
        }
        TextField {
            id : modelNameText
            anchors.left: modelNameLabel.right
            height : 20
            width : parent.width - modelNameLabel.width - 9
        }
    }
    Rectangle {
        id: descriptionItem
        x : 5
        height : 22
        width : parent.width - 5;
        color : background1;
        Text {
            id : modelDescriptionLabel
            height : 20
            text : qsTr("Description");
            wrapMode:Text.Wrap
        }
        TextArea {
            id: modelDescriptionText
            anchors.left: modelDescriptionLabel.right
            height : 60
            width : parent.width - modelDescriptionLabel.width  - 9
        }
    }
}
