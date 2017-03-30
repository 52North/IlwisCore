import QtQuick 2.0
import UIContextModel 1.0

ComboxLabelPair {
    property string ilwisType
    property string query : "(datatype & %1)!=0 and (readwrite='rc' or readwrite='rcu')"

    onQueryChanged: {
        itemModel = uicontext.formatList(query, ilwisType)
        if ( itemModel.length === 0){
            enabled : false
            select("")
        }
    }

    function selectedFormatString() {
        var expr = "name='" + comboText + "'";
        var result = uicontext.formatList(expr, "format")
        if ( result.length > 0)
            return result[0]
        return ""

    }

    Component.onCompleted: {
        itemModel = uicontext.formatList(query, ilwisType)
    }
}
