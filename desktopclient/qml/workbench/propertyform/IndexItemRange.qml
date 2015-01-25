import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import ResourceModel 1.0
import IlwisObjectModel 1.0
import DomainItemModel 1.0

import "../../Global.js" as Global

Text {
    width: 100
    height: Global.rowHeight
    text : dimensions + qsTr(" items");
}
