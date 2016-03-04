import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

ComboBox{
    id : catalogfilters
    anchors.leftMargin: 2
    anchors.topMargin: 2
    width : 120
    height : 22
    model : mastercatalog.defaultFilters
    textRole: "name"
    onCurrentIndexChanged: {
        if ( tabmodel)
            tabmodel.selectTab()
        if ( currentIndex > 0){ // first entry is a default empty one
            var filter = model[currentIndex].catalogQuery
            var url = model[currentIndex].url
            if ( url === "")
                url = "ilwis://mastercatalog"
            mastercatalog.selectedBookmark(url)
            bigthing.changeCatalog(filter,"catalog", url)
        }

    }
}

