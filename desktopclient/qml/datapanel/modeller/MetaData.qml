import QtQuick 2.0
import QtQuick.Controls 1.1;
import QtQuick.Layouts 1.1;
import "../../Global.js" as Global



Rectangle {
    id : metadata
    width : parent.width
    height : parent.height
    color : Global.formBackGround

    function setName(name){
        valueName.text = name
    }

    function setDesc(desc){
        valueDesc.text = desc
    }

    function setKeywords(words){
        words = words.split(", ")
        keywords.clear()
        for (var i = 0; i < words.length; i++) {
            keywords.append({'xWord': words[i]})
        }
    }

    Rectangle {
        width : parent.width
        height : 22
        color : Global.alternatecolor1

        Text {
            id : keyName
            width : 100
            height : parent.height
            text : qsTr("Name:")
            font.bold : true
        }
        Text {
            id : valueName
            anchors.left: keyName.right
            height : parent.height
            width : parent.width - keyName.width - 9
        }
    }

    Rectangle {
        y: 20
        width : parent.width
        height : 40
        ListModel { id: keywords }

        Text {
            id : keyWords
            width : 100
            height : parent.height
            text : qsTr("Keywords:")
            font.bold : true
        }
        ListView{
            id : keywordsList
            anchors.left: keyWords.right
            height : parent.height
            width : parent.width - keyName.width - 9

            clip : true
            model : keywords
            delegate: Item {
                width: parent.width
                height: 10
                Text {
                    width: parent.width
                    height: parent.height
                    text: xWord
                }
            }
        }
    }

    Rectangle {
        y: 60
        width : parent.width
        height : 60
        color : Global.alternatecolor1

        Text {
            id : keyDesc
            width : 100
            height : parent.height
            text : qsTr("Description:")
            font.bold : true
        }
        Text {
            id : valueDesc
            anchors.left: keyDesc.right
            height : 60
            width : parent.width - keyDesc.width - 9
            wrapMode: Text.Wrap
        }
    }

}

