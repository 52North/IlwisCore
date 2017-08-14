import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Controls.DropableItem{
    id : dropItem
    width : 250
    height : 0

    clip:true


    Component {
        id : itemresolution
        Controls.TextEditLabelPair{
            labelText: qsTr("Resolution")
            labelWidth: 100
            height : 20
            width : parent.width
            regexvalidator: /^\d*(\.\d*)?$/
        }
    }

    Rectangle {
        id : container
        height: 590
        width : parent.width
        border.width : 1
        border.color : Global.edgecolor
        radius: 5
        property var parentDomain
        property var itemArray : []



        ItemDomainCommonPart{
            id : commonpart
            domaintype: "itemdomain"
            valuetype: "Numeric Interval"
            parentItemList : "SelectNumericInterval.qml"
            newItemEditor: "AddNewNumericInterval.qml"
            domitems.source : "IntervalTable.qml"
            additionalFields.sourceComponent : itemresolution
        }

        function apply(overwrite) {
            var itemstring = ""
            if ( commonpart.domitems.item.model){
                if (!overwrite){
                    if ( mastercatalog.exists("ilwis://internalcatalog/"+ commonpart.name, "coordinatesystem")){
                        return false;
                    }
                }
                for(var i = 0; i < commonpart.domitems.item.model.length; ++i){
                    if (itemstring !== "")
                        itemstring += "|"
                    itemstring += commonpart.domitems.item.model[i].name;
                    if (  commonpart.parentdomain == "" ){
                        itemstring += "|"+ commonpart.domitems.item.model[i].minvalue;
                        itemstring += "|"+ commonpart.domitems.item.model[i].maxvalue;
                        itemstring += "|"+ commonpart.domitems.item.model[i].code;
                        itemstring += "|"+ commonpart.domitems.item.model[i].description;

                    }
                }
                var url = applyButton.currentCatalogCorrectUrl() + "/"+ commonpart.name
                var res = commonpart.additionalFields.item.content
                var createInfo = {parentdomain : commonpart.parentdomain, type : "itemdomain", valuetype : "interval", name : url, resolution : res,  items : itemstring, description : commonpart.description,strict : commonpart.strict}
                var ilwisid = objectcreator.createObject(createInfo)
            }
            return true
        }

        ApplyCreateButtons {
            width : parent.width
            height : 60
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            id : applyButton
            createObject: container.apply

        }

    }
    function addDomainItems(items,clear){
        if ( clear)
           container.itemArray = []
        for(var i = 0; i < items.length; ++i){
            if ( items[i].name === "")
                continue;
            if ( Number(items[i].minvalue) > Number(items[i].maxvalue)){
                continue;
            }
            var illegal = false
            for ( var j = 0; j < container.itemArray.length; ++j){
                // double names allowed
                if( items[i].name === container.itemArray[j].name){
                    uicontext.addMessage(qsTr("Duplicate names are not allowed"),"warning")
                    illegal = true
                }
                // no double codes allowed unless its empty
                if( items[i].code !== "" && (items[i].code === container.itemArray[j].code)){
                    uicontext.addMessage(qsTr("Duplicate codes are not allowed"),"warning")
                    illegal = true
                }
                if ( illegal)
                    break
            }
            if ( !illegal){
                var found = false
                for(var n=0; n < container.itemArray.length; ++n){
                    if ( Number(items[i].minvalue) > Number(container.itemArray[n].minvalue)){
                        container.itemArray.splice(n,0, items[i])
                        found = true
                        break
                    }
                }
                if (!found)
                    container.itemArray.push(items[i])
            }
        }
        commonpart.domitems.item.model = container.itemArray
    }


}




