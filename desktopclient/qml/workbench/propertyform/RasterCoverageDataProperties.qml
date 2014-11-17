import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

Rectangle {
    id : container
    anchors.fill : parent

    AttributeProperties{
        domainUsed: domainName
        valueTypeUsed: valuetype
    }
}
