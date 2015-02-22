import QtQuick 2.0
import QtQuick.Controls 1.0
import "../../Global.js" as Global

Item {
    width : parent.width
    height : 22
    id : actualRange
    Item {
        width: 100;
        id : line4;
        height : Global.rowHeight;
        Button {
            text : qsTr("Actual range");
            width : 80
            height : 16
            onClicked: {
                actualRangeLoader.source = ""
                var rangestring = rangeDefinition(false,true,columnName)
                actualRangeLoader.setSource(rangeFieldType(rangestring),{"useDefaultRange":false})
            }
        }
    }
    Loader { id: actualRangeLoader
        width: 164
        anchors.left: line4.right
        anchors.top : parent.top
        height:Global.rowHeight
        Component.onCompleted:{ actualRangeLoader.setSource(rangeFieldType(rangeDefinition(false,false,columnName)),{"useDefaultRange":false})}
    }
}


