import QtQuick 2.2
import QtQuick.Controls 1.0

Row {
    width: conditionList.width
    Column {
        Text {
            horizontalAlignment: Text.AlignHCenter
            text: String.fromCharCode(xId * 2 + 65)
            width: (conditionList.width - 50) / 4
            font.strikeout: first
            font.bold: !first
        }
    }
    Column {
        Text {
            horizontalAlignment: Text.AlignHCenter
            text: condition === '' ? '[operator]' : condition
            width: (conditionList.width - 50) / 2
        }
    }
    Column {
        Text {
            horizontalAlignment: Text.AlignHCenter
            text: String.fromCharCode(xId * 2 + 65 + 1)
            width: (conditionList.width - 50) / 4
            font.strikeout: second
            font.bold: !second
        }
    }
}
