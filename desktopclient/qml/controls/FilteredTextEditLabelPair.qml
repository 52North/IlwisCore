import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0
import "../Global.js" as Global

TextEditLabelPair {
    property string filterImage
    property string filterType
    property bool useCreateButton : false
    signal createClicked()
    editWidth : parent.width - ( useCreateButton ? Global.rowHeight + 3 : 2*Global.rowHeight + 3)


    Button{
        id : filterBut
        width : Global.rowHeight
        height:Global.rowHeight
        checkable : true
        checked : false
        anchors.right : parent.right
        anchors.rightMargin: 2
        Image{
            id : filterImg
            anchors.centerIn : parent;
            width : Global.rowHeight - 6;
            height:Global.rowHeight - 6;
            source:filterImage;
            fillMode: Image.PreserveAspectFit
        }
        ColorOverlay {
            id : overlay
            anchors.fill: filterImg
            source: filterImg
            color: "#60000000"
            visible : false
        }

        onClicked : {
            mastercatalog.currentCatalog.filterChanged(filterType + "|exclusive" , checked)
            overlay.visible = filterBut.checked
        }
    }
    Button{
        width : useCreateButton ? Global.rowHeight : 0
        height: useCreateButton ? Global.rowHeight : 0
        visible: useCreateButton
        enabled: useCreateButton
        anchors.right: filterBut.left
        checkable : true
        checked : false
        anchors.rightMargin: 2
        Image{
            anchors.centerIn : parent;
            width : Global.rowHeight - 6;
            height:Global.rowHeight - 6;
            source: "../images/plus20.png"
            fillMode: Image.PreserveAspectFit
        }

        onClicked : {
            createClicked()
        }
    }

}

