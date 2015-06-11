import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import LayersView 1.0
import "../../workbench/propertyform" as MetaData
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base

LayersView {

    objectName : "overview_mainui"


    Component.onCompleted: {
        manager = uicontext.createLayerManager(objectName)
        overview.setManager(manager)
        renderer.associate(objectName,"drawEnded")
    }

    function finalizeDraw(){
        if ( metatdata.manager && layerview.manager){
            var envelope = renderer.attributeOfDrawer("selectiondrawer","envelope")
            var env = {envelope : envelope, preserveaspectration : false}
            overview.setAttribute("selectiondrawer", env )
            overview.update()
        }

    }
}

