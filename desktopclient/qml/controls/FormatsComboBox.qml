import QtQuick 2.0
import UIContextModel 1.0

ComboxLabelPair {
    property string ilwisType
    property string query : "(datatype & %1)!=0 and (readwrite='rc' or readwrite='rcu')"

    Component.onCompleted: {
        itemModel = uicontext.formatList()
    }
}
