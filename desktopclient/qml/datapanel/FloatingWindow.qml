import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Window 2.1


Window {
    property string datapanel

    function transfer(datap){
        if ( content.item){
            content.item.transfer(datap)
        }
    }

    function addDataSource(filter,outputtype, url){
        if ( content.item){
            content.item.addDataSource(filter,outputtype, url)
        }
    }

    Loader {
        id : content
        width : parent.width
        height : parent.height
        source : datapanel
    }
}

