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
    x : 4
    clip:true
    property var combotriplets : []
    property var xvalues : []
    property var yvalues : []
    property bool combosAreItem : true

    function partOf(values, name){
        for(var i=0; i < values.length; ++i){
            if ( values[i] === name)
                return true;
        }
        return false;
    }

    function apply(overwrite){
        var url = applyBut.currentCatalogCorrectUrl() + "/"+ objectcommon.itemname
        var xstrings="", ystrings="", combos=""
        for(var i=0; i < xvalues.length; ++i){
            if ( xstrings !== "") xstrings += "|"
            xstrings += xvalues[i]
        }
        for( i=0; i < yvalues.length; ++i){
             if ( ystrings !== "") ystrings += "|"
            ystrings += yvalues[i]
        }
        for(i=0; i < combotriplets.length; ++i){
             if ( combos !== "") combos += "|"
            combos += combotriplets[i]
        }
        var createinfo = { name : url, type : "combinationmatrix", xaxisdomain :xaxisDomain.content,
                                                                                    yaxisdomain :yaxisDomain.content,
                                                                                    combodomain :cDomain.content,
                                                                                    xaxis : xstrings,
                                                                                    yaxis : ystrings,
                                                                                    combinations : combos,
                                                                                    description :objectcommon.description}
        var createdId = objectcreator.createObject(createinfo)
        if ( createdId !== "?" && editorList.depth > 1){
            editorList.currentItem.setValue("combinationmatrix", createdId)
        }
        return true
    }

    function value(xindex,yindex){
        var xname = xvalues[ xindex]
        var yname = yvalues[ yindex]
        for(var i=0; i < combotriplets.length; ++i)    {
            if ( combotriplets[i].indexOf(xname + "|" + yname) === 0){
                var triplet = combotriplets[i]
                var parts = triplet.split("|")
                if ( parts.length === 3){
                    return parts[2]    ;
                }
            }

        }
        return "?"
    }

    function findtriplet(xindex,yindex,cindex){
        if ( xindex === -1 || yindex === -1)
            return ""
        var xname = xaxisvalues.model[xindex]
        var yname = yaxisvalues.model[yindex]
        var recalc = false
        for(var i=0; i < combotriplets.length; ++i)    {
            if ( combotriplets[i].indexOf(xname + "|" + yname) === 0){
                var triplet = combotriplets[i]
                var parts = triplet.split("|")
                if ( parts.length === 3){
                    var cname = cindex !== -1 ? combovalues.model[cindex] : parts[2]
                    var newtriplet = xname + "|" + yname + "|" + cname
                    combotriplets.splice(i,1)
                    combotriplets.push(newtriplet)
                    combotriplets.sort()
                    if ( !partOf(xvalues, xname)){
                        xvalues.push(xname)
                    }
                    if ( !partOf(yvalues, yname)){
                        yvalues.push(yname)
                    }
                    grid.recalc()

                    return parts[2];
                }
            }
        }
        if ( cindex !== -1){
            cname = combovalues.model[cindex]
            newtriplet = xname + "|" + yname + "|" + cname
            combotriplets.push(newtriplet)
            combotriplets.sort()
            if ( !partOf(xvalues, xname)){
                xvalues.push(xname)
                recalc = true
            }
            if ( !partOf(yvalues, yname)){
                yvalues.push(yname)
                recalc = true
            }
            if ( recalc)
                grid.recalc()
            return cname
        }

        return ""
    }

    Column {
        id : dataarea

        width : parent.width - 5
        height : 600
        spacing : 4
        EditorHeader{}
        IlwisObjectCommon{
            id : objectcommon
        }

        Controls.TextEditLabelPair{
            id : xaxisDomain
            width : parent.width - 10
            keys : ["itemdomain"]
            content: ""
            labelText : qsTr("X Axis domain")
            labelWidth: 120
            onIlwisobjectidChanged:  {
                var domain = mastercatalog.id2object(ilwisobjectid,0)
                if ( domain){
                    if ( domain.typeName !== "itemdomain")
                        return
                    var items = domain.domainitems
                    var itemNames = []
                    for(var i=0; i < items.length; ++i){
                        itemNames.push(items[i].name)
                    }
                    itemNames.push("?")
                    xaxisvalues.model = itemNames
                    xaxisvalues.currentIndex = -1
                }
            }
        }
        Controls.TextEditLabelPair{
            id : yaxisDomain
            width : parent.width - 10
            keys : ["itemdomain"]
            content: ""
            labelText : qsTr("Y Axis domain")
            labelWidth: 120
            onIlwisobjectidChanged:  {
                var domain = mastercatalog.id2object(ilwisobjectid,0)
                if ( domain){
                    if ( domain.typeName !== "itemdomain")
                        return
                    var items = domain.domainitems
                    var itemNames = []
                    for(var i=0; i < items.length; ++i){
                        itemNames.push(items[i].name)
                    }
                    itemNames.push("?")
                    yaxisvalues.model = itemNames
                    yaxisvalues.currentIndex = -1
                }
            }
        }
        Controls.TextEditLabelPair{
            id : cDomain
            width : parent.width - 10
            keys : ["itemdomain","numericdomain"]
            content: ""
            labelText : qsTr("Combo domain")
            labelWidth: 120
            onIlwisobjectidChanged:  {
                var domain = mastercatalog.id2object(ilwisobjectid,0)
                if ( domain){
                    if ( domain.typeName === "itemdomain" || domain.typeName === "numericdomain"){

                        var itemNames = []
                        if ( domain.typeName === "itemdomain"){
                            var items = domain.domainitems
                            for(var i=0; i < items.length; ++i){
                                itemNames.push(items[i].name)
                            }
                            itemNames.push("?")

                        }else{
                            combosAreItem = false
                            for(var i=0; i<1000; ++i)
                                itemNames.push("?")
                        }
                        combovalues.model = itemNames
                        combovalues.currentIndex = -1
                    }
                }
            }

        }
        Row {
            spacing: 4
            width : parent.width - 10
            x : 4
            height : 180
            Column {
                height : parent.height
                width : parent.width / 3
                Text {
                    text : qsTr("X Axis values")
                    width : 100
                    height : 18
                }
                Rectangle {
                    width : parent.width
                    height : parent.height - 22
                    border.width : 1
                    border.color : Global.edgecolor

                    ListView {
                        id :xaxisvalues
                        anchors.fill: parent
                        clip : true
                        currentIndex : -1
                        highlight: Rectangle{ width :( xaxisvalues.currentIndex >= 0 ? parent.width : 0); height : 18; color : Global.selectedColor}
                        delegate: Text{
                            x : 3
                            width : parent.width
                            height : 18
                            text : modelData
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    xaxisvalues.currentIndex = index
                                    var combo = findtriplet(index, yaxisvalues.currentIndex, combovalues.currentIndex)
                                    combovalues.select(combo)
                                }
                            }
                        }
                    }
                }
            }
            Column {
                height : parent.height
                width : parent.width / 3
                Text {
                    text : qsTr("Y Axis values")
                    width : 100
                    height : 18
                }
                Rectangle {
                    width : parent.width
                    height : parent.height - 22
                    border.width : 1
                    border.color : Global.edgecolor
                    ListView {
                        id :yaxisvalues
                        clip : true
                        currentIndex : -1
                        anchors.fill: parent
                        highlight: Rectangle{ width:( yaxisvalues.currentIndex >= 0 ? parent.width : 0); height : 18; color : Global.selectedColor}
                        delegate: Text{
                            x: 3
                            width : parent.width
                            height : 18
                            text : modelData
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    yaxisvalues.currentIndex = index
                                    var combo = findtriplet(xaxisvalues.currentIndex,index, combovalues.currentIndex)
                                    combovalues.select(combo)

                                }
                            }
                        }
                    }
                }
            }
            Column {
                height : parent.height
                width : parent.width / 3 - 5
                Text {
                    text : qsTr("Combination values")
                    width : 100
                    height : 18
                }
                Rectangle {
                    width : parent.width
                    height : parent.height - 22
                    border.width : 1
                    border.color : Global.edgecolor
                    ListView {
                        id :combovalues
                        function select(combo){
                            for(var i=0; i < model.length; ++i){
                                if ( model[i] === combo){
                                    currentIndex = i
                                    break;
                                }
                            }
                        }

                        clip : true
                        currentIndex : -1
                        anchors.fill: parent
                        highlight: Rectangle{
                            width :( combovalues.currentIndex >= 0 ? parent.width : 0); height : 18; color : Global.selectedColor
                         }
                        delegate: TextInput{
                            id : tf
                            x: 3
                            width : parent.width
                            height : 18
                            text : modelData
                              MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    combovalues.currentIndex = index
                                    findtriplet(xaxisvalues.currentIndex,yaxisvalues.currentIndex, index)
                                }
                            }
                        }
                    }
                }
            }
        }
        GridView {
            id : grid
            function cellW(){
                // var xcellsize = parent.width / (xvalues.length + 1) - 20
                // return xcellsize
                return 100
            }

            function recalc(){
                var xcellsize = grid.cellW()
                width = xcellsize * (xvalues.length + 1)
                height = 20 * (yvalues.length + 1)
                var newmodel =[]
                newmodel.push(" ")
                for(var x=0; x < xvalues.length; ++x){
                    newmodel.push(xvalues[x])
                }

                for(var y=0 ; y < yvalues.length; ++y){
                    newmodel.push(yvalues[y])
                    for( x=0; x < xvalues.length; ++x){
                        newmodel.push( value(x,y))
                    }
                }
                model = newmodel
            }
            width :  grid.cellW() * (xvalues.length + 1)
            cellWidth: grid.cellW()
            cellHeight: 20
            model : combotriplets
            delegate: Rectangle {
                width : grid.cellW()
                height : 20
                border.width: 1
                border.color: Global.edgecolor
                color : index < xvalues.length + 1 ? uicontext.lightColor :( index % (xvalues.length + 1) == 0 ? uicontext.paleColor :  "white")
                Text{
                    x: 2
                    y : 2
                    height : 18
                    width : parent.width - 3
                    text : modelData
                    clip : true
                    elide : Text.ElideMiddle
                }
            }
        }

    }
    ApplyCreateButtons {
        id : applyBut
       width : parent.width
       height : 60
       anchors.bottom: parent.bottom
       anchors.bottomMargin: 8
       createObject: dropItem.apply
   }
    Component.onCompleted: {
        //dummy itemnames
    }

}
