import QtQuick 2.0
import UIContextModel 1.0

ComboxLabelPair {
    property string ilwisType
    property string query : "(datatype & %1)!=0 and (readwrite='rc' or readwrite='rcu')"

    function selectedFormatString() {
        var expr = "query = name='" + comboText + "'";
        var result = uicontext.formatList(query, "provider")
        if ( result.length > 0)
            return requestedType + ":" + result[0]
        return null

    }

    Component.onCompleted: {
        itemModel = uicontext.formatList(query, ilwisType)
    }
}
