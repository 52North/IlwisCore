import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1


Item {
    property var operation
    property int columnIndex

    anchors.fill : parent
    Column {
        width : 120
        height : 60
        spacing : 10
        Button{
            text : qsTr("Ascending");
            height : 20
            width : 100
            onClicked: {
                var paramaters = {columnindex: columnIndex,order:"ascending"}
                operation.execute(paramaters)
            }
        }
        Button{
            text : qsTr("Decending");
            height : 20
            width : 100
        }
    }
}

