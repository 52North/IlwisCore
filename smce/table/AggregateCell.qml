import QtQuick 2.0


Text {
    text: styleData.value
    verticalAlignment:Text.AlignVCenter
    color : styleData.selected ? "white" : "black"
    elide: Text.ElideMiddle
}


