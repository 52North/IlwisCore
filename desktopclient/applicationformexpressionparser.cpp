#include "kernel.h"
#include "mastercatalog.h"
#include "models/resourcemodel.h"
#include "dataformat.h"
#include "applicationformexpressionparser.h"

using namespace Ilwis;

ApplicationFormExpressionParser::ApplicationFormExpressionParser()
{
}

ApplicationFormExpressionParser::FormParameter ApplicationFormExpressionParser::addParameter(const Resource& resource,
                                                                                             quint32 index,
                                                                                             const QStringList& choices,
                                                                                             bool optional, int optionGroup, const QString& defvalue) const{
    FormParameter parm;
    QString prefix = QString("pin_%1_").arg(index + 1);
    parm._label = resource[prefix + "name"].toString();
    parm._dataType = resource[prefix + "type"].toULongLong();
    parm._isOptional = optional;
    parm._optionGroup = optionGroup;
    parm._choiceList = choices;
    parm._defValue = defvalue;
    if ( choices.size() > 4){
        parm._fieldType = ftCOMBOBOX;
    }
    else if ( choices.size() > 0){
        parm._fieldType = ftRADIOBUTTON;
        quint64 bits = ~itINTEGER;
        quint64 result = parm._dataType & bits;
        if ( result != 0){ // if there are other things than integers in the choice types; only integers is a 'real'choice list
            if ( hasType(parm._dataType, itCOVERAGE | itNUMBER |itTABLE )){
                parm._fieldType |= ftTEXTEDIT;
            }
        }
    }
    else if ( parm._dataType == itBOOL){
        parm._fieldType = ftRADIOBUTTON;
        parm._choiceList = {"!yes", "no"};
    }
    else
        parm._fieldType = ftTEXTEDIT;
    return parm;
}

void ApplicationFormExpressionParser::setParameter(const Resource& resource, bool& inChoiceList,
                                                   std::vector<FormParameter>& parameters, QString& part,
                                                   QStringList& choices, int& parmCount,
                                                   bool isOptional, int optionGroup,
                                                   const QString& defvalue = "") const
{
    if ( inChoiceList)
        choices << part;
    parameters.push_back(addParameter(resource, parmCount, choices, isOptional, optionGroup, defvalue));
    choices.clear();
    part = "";
    inChoiceList = false;
    ++parmCount;
}

std::vector<ApplicationFormExpressionParser::FormParameter> ApplicationFormExpressionParser::getOutputParameters(const Ilwis::Resource &resource) const{
    std::vector<FormParameter> parameters;
    QString outparms = resource["outparameters"].toString();
    bool isService = resource["keyword"].toString().indexOf("service") != -1;
    if ( outparms == "0")
        return parameters;

    for ( int order = 0; order < outparms.toInt(); ++order){
        QString prefix = "pout_" + QString::number(order + 1) + "_";
       FormParameter parm;
       parm._fieldType = isService ? ftTEXTAREA :  ftTEXTEDIT;
       parm._dataType = resource[prefix + "type"].toULongLong();
       parm._isOptional = false;
       parm._label = resource[prefix + "name"].toString();
       parameters.push_back(parm);

    }

    return parameters;
}
std::vector<ApplicationFormExpressionParser::FormParameter> ApplicationFormExpressionParser::getParameters(const Resource& resource) const
{
    std::vector<FormParameter> parameters;
    QString expression = resource["syntax"].toString();
    qint32 index = expression.indexOf("(");
    expression = expression.mid(index + 1, expression.size() - index - 2);
    QString part;
    int parmCount = 0;
    bool inChoiceList = false;
    bool isOptional = false;
    bool checkGroup;
    int optionGroup = 0;
    QString specials = "[]|,";
    QStringList choices;
    for(auto c : expression) {
        if ( specials.indexOf(c) == -1){
            part += c;
            continue;
        }
        if ( c == ',' && !part.isEmpty()){
            part = part.trimmed();
            QString defvalue =  part[0]=='!' ? "true" : "";
            setParameter(resource, inChoiceList, parameters, part, choices, parmCount, isOptional, optionGroup,defvalue);
        } else if ( c == '['){
            if ( !part.isEmpty() ) {
                part = part.trimmed();
                setParameter(resource, inChoiceList, parameters, part, choices, parmCount, isOptional,optionGroup);
                checkGroup = true;
            }
            isOptional = true;
        } else if ( c == ']'){
            if ( !part.isEmpty()) {
                part = part.trimmed();
                QString defvalue =  part[0]=='!' ? "true" : "";
                setParameter(resource, inChoiceList, parameters, part, choices, parmCount, isOptional, optionGroup,defvalue);
            }
            checkGroup = false;
            isOptional = false;
            ++optionGroup;
        } else if ( c == '|'){
            if ( part.indexOf("=") !=-1){
                choices << part.split("=")[1];
            }else
                choices << part.trimmed();
            inChoiceList = true;
        }
        part.clear();
    }
    // last parameter
    if (part != ""){
        part = part.trimmed();
        setParameter(resource, inChoiceList, parameters, part, choices, parmCount, isOptional,optionGroup);
    }
    return parameters;

}

QString ApplicationFormExpressionParser::iconName(IlwisTypes dataType) const{
    if ( hasType(dataType, itRASTER)){
        return ResourceModel::iconPath(itRASTER);
    }
    if ( hasType(dataType, itFEATURE)){
        return ResourceModel::iconPath(itFEATURE);
    }
    if ( hasType(dataType, itTABLE)){
        return ResourceModel::iconPath(itTABLE);
    }
    if ( hasType(dataType, itNUMBER)){
        return ResourceModel::iconPath(itNUMBER);
    }
    if ( hasType(dataType, itSTRING)){
        return ResourceModel::iconPath(itSTRING);
    }
    if ( hasType(dataType, itDOMAIN)){
        return ResourceModel::iconPath(itDOMAIN);
    }
    return sUNDEF;
}

QString ApplicationFormExpressionParser::keys(IlwisTypes type) const{
    QString keypart;
    if ( hasType(type, itRASTER)){
        keypart += "\""+ TypeHelper::type2name(itRASTER) + "\"";
    }
    if ( hasType(type, itTABLE)){
        if ( keypart != "") keypart += ",";
        keypart += "\""+ TypeHelper::type2name(itTABLE) + "\"";
    }
    if ( type == itPOINT){
        if ( keypart != "") keypart += ",";
        keypart += "\""+ TypeHelper::type2name(itPOINT) + "\"";
    }
    if ( type == itLINE){
        if ( keypart != "") keypart += ",";
        keypart += "\""+ TypeHelper::type2name(itLINE) + "\"";
    }
    if ( type == itPOLYGON){
        if ( keypart != "") keypart += ",";
        keypart += "\""+ TypeHelper::type2name(itPOLYGON) + "\"";
    }
    if ( type == itFEATURE){
        if ( keypart != "") keypart += ",";
        keypart += "\""+ TypeHelper::type2name(itFEATURE) + "\"," + "\""+ TypeHelper::type2name(itPOINT) + "\"," + "\""+ TypeHelper::type2name(itLINE) + "\"," + "\""+ TypeHelper::type2name(itPOLYGON) + "\"";
    }
    if ( hasType(type, itDOMAIN)){
        if ( keypart != "") keypart += ",";
        keypart += "\""+ TypeHelper::type2name(itDOMAIN) + "\"";
    }
    if ( hasType(type, itNUMBER)){
        if ( keypart != "") keypart += ",";
        keypart += "\""+ TypeHelper::type2name(itNUMBER) + "\"";
    }
    if ( hasType(type, itSTRING)){
        if ( keypart != "") keypart += ",";
        keypart += "\""+ TypeHelper::type2name(itSTRING) + "\"";
    }
    return keypart;
}

QString ApplicationFormExpressionParser::setInputIcons(const QString& iconField1, const QString& iconField2,const std::vector<FormParameter>& parameters, int i, int& imagewidth) const
{
    const int iconsize = 20;
    QString imagePart;
    imagewidth = 4;
    if ( hasType(parameters[i]._dataType, itRASTER)){
        imagePart += QString(iconField1).arg(iconName(itRASTER)).arg(IlwisObject::type2Name(itRASTER));
        imagewidth += iconsize;
    }
    if ( hasType(parameters[i]._dataType,  itTABLE)){
        imagePart += QString(iconField1).arg(iconName(itTABLE)).arg(IlwisObject::type2Name(itTABLE));
        imagewidth += iconsize;
    }
    if ( hasType(parameters[i]._dataType, itFEATURE)){
        imagePart += QString(iconField1).arg(iconName(itFEATURE)).arg(IlwisObject::type2Name(itFEATURE));
        imagewidth += iconsize;
    }
    if ( hasType(parameters[i]._dataType, itDOMAIN)){
        imagePart += QString(iconField1).arg(iconName(itDOMAIN)).arg(IlwisObject::type2Name(itDOMAIN));
        imagewidth += iconsize;
    }
    if ( hasType(parameters[i]._dataType, itNUMBER)){
        imagePart += QString(iconField2).arg(iconName(itNUMBER));
        imagewidth += iconsize;
    }
    if ( hasType(parameters[i]._dataType, itSTRING)){
        imagePart += QString(iconField2).arg(iconName(itSTRING));
        imagewidth += iconsize;
    }
    if ( imagePart != ""){
        imagePart = QString("Row{width:%1;spacing:2;height:20 ;anchors.right: parent.right;anchors.rightMargin: 2;%2}").arg(imagewidth).arg(imagePart) ;
    }
    return imagePart;
}

QString ApplicationFormExpressionParser::makeFormPart(int width, const std::vector<FormParameter>& parameters, bool input, QString& results, bool showEmptyOptionInList, QString invisibleFieldIndexes) const{
    QStringList invisibleFieldList;
    if(!invisibleFieldIndexes.isEmpty()){
        invisibleFieldList = invisibleFieldIndexes.split("|");
    }

    QString rowBodyText = "Rectangle{visible:%7;height : 30;width : parent.width;color : \"white\";%1Text { x:%5 + %6;maximumLineCount: 2;text: qsTr(\"%2\"); id:label_pin_%4; width : %3 - %5 - %6;wrapMode:Text.Wrap; }";

    QString textField = "DropArea{ x : %2; height : 20; width : parent.width - label_pin_%1.width - 5 - %3 - %4 - %5; keys: [%6];\
               onDropped : { pin_%1.text = drag.source.message }\
            TextField{ id : pin_%1; anchors.fill : parent optionalOutputMarker %7}}";
    QString textArea = "DropArea{ x : %2; height : 55; width : parent.width - label_pin_%1.width - 5 - %3 - %4 - %5; keys: [%6];\
           onDropped : { pin_%1.text = drag.source.message }\
        TextArea{ id : pin_%1; anchors.fill : parent optionalOutputMarker %7}}";

   // QString textArea = "TextArea{ id : pin_%1; x : %2 + %5; height : 55; width : parent.width - label_pin_%1.width - 5 - %3 - %4 - %5 optionalOutputMarker}";
    QString iconField1 = "Button{ width : 20; height:20; checkable : true;checked : false;"
            "onClicked : {mastercatalog.currentCatalog.filterChanged(\"%2|exclusive\" , checked)}"
            "Image{anchors.centerIn : parent;width : 14; height:14;source:\"../images/%1\";fillMode: Image.PreserveAspectFit}}";
    QString iconField2 = "Image{width : 14; height:14;source:\"../images/%1\";fillMode: Image.PreserveAspectFit}";
    QString comboField = "ComboBox{id : pin_%1; x : %2;width : parent.width - label_pin_%1.width - 5 - %3;model : %4";
    QString rowBodyChoiceHeader = "Row{ width : parent.width;Text { text: qsTr(\"%1\"); width : %2; } Column{ExclusiveGroup { id: exclusivegroup_pin_%3} %4}}";
    QString rowChoiceOption = "RadioButton{id:choice_pin_%1;text:qsTr(\"%2\");checked:%3;exclusiveGroup:exclusivegroup_pin_%4;property string value:qsTr(\"%5\")}";
    QString formRows;
    int oldOptionGroup = -1;
    int xshift = 0;
    for(int i = 0; i < parameters.size(); ++i){
        QString visibile = "true";
        for(int j=0;j<invisibleFieldList.size();++j){
            if(i==invisibleFieldList[j].toInt()){
                visibile = "false";
            }
        }

        QString check;
        if ( parameters[i]._isOptional){
            if ( oldOptionGroup != parameters[i]._optionGroup){
                check = QString("CheckBox{id:check_pin_%1; checked:%2;width:20}").arg(i).arg(parameters[i]._defValue == "" ? "false" : "true");
                oldOptionGroup = parameters[i]._optionGroup;
            } else
               xshift = 22;

        }else{
            xshift = 0;
        }
        int checkWidth = check == "" ? 0 : 20;
        int imagewidth = 0;
        if ( (hasType(parameters[i]._fieldType,ftTEXTEDIT | ftTEXTAREA)) || (parameters[i]._fieldType == (ftTEXTEDIT | ftRADIOBUTTON))){
            QString imagePart = setInputIcons(iconField1, iconField2, parameters, i,imagewidth);
            QString checkEffects;
            if ( check != ""){
                checkEffects=QString(";enabled: check_pin_%1.checked;opacity:check_pin_%1.checked?1 :0.25").arg(i);
            }

            QString textFieldPart = QString(hasType(parameters[i]._fieldType,ftTEXTEDIT) ? textField : textArea).arg(i).
                    arg(width).
                    arg(checkWidth).
                    arg(imagewidth).
                    arg(xshift).
                    arg(input ? keys(parameters[i]._dataType) : "\"?\"").
                    arg(checkEffects);

            QString parameterRow = QString(rowBodyText + textFieldPart + imagePart + "}").arg(check).arg(parameters[i]._label).arg(width).arg(i).arg(checkWidth).arg(xshift).arg(visibile);
            formRows += parameterRow;
            if ( results != "")
                results += "+ \"|\" +";
            results += QString(input ? "pin_%1.text" : "pout_%1.text").arg(i);
            if ( !input){
                QString query = QString("(datatype & %1)!=0 and (readwrite='rc' or readwrite='rcu')").arg(parameters[i]._dataType);
                QString formatList = formats(query, parameters[i]._dataType);
                if ( formatList != ""){
                    QString formatLabel = QString("Row{height:20;width:parent.width;Text { x:5;text: qsTr(\"Output format\"); id:label_pout_format_%2; width :%1;}").arg(width).arg(i);
                    QString formatCombo = QString("ComboBox{id : pout_format_%1; height:20; width : parent.width - label_pout_format_%1.width - 5;model : %2}").arg(i).arg(formatList);
                    results += "+\"@@\"+"  + QString("pout_format_%1.currentText").arg(i);
                    formRows +=formatLabel + formatCombo + "}";
                }
            }

        }
        if ( parameters[i]._fieldType == ftRADIOBUTTON){
            QString buttons;
            QString state = "true";
            if ( showEmptyOptionInList ) {
                buttons += QString(rowChoiceOption).arg(QString::number(i) + "empty_value").arg("- (empty)").arg("true").arg(i).arg("");
            }
            for(auto choiceString : parameters[i]._choiceList){
                QString choice = choiceString, state="false";
                if (choice[0] == '!'){
                    choice = choice.mid(1);
                    if (buttons.isEmpty()) state = "true";
                }
                buttons += QString(rowChoiceOption).arg(QString::number(i) + choice).arg(choice).arg(state).arg(i).arg(choice);
            }
            formRows += QString(rowBodyChoiceHeader).arg(parameters[i]._label).arg(width).arg(i).arg(buttons);
            if ( results != "")
                results +=  "+ \"|\" +";
            QString field = QString("exclusivegroup_pin_%1.current.value").arg(i);
            results += field;
        }
        if ( parameters[i]._fieldType == ftCOMBOBOX){
            QString choices = "[";
            for(auto choiceString : parameters[i]._choiceList){
                if ( choices.size() > 1)
                    choices += ",";
                else if ( showEmptyOptionInList )
                    choices += "\" \",";
                if ( choiceString[0] == '!'){
                    choiceString = choiceString.mid(1);
                }
                choices += "\"" + choiceString + "\"";
            }
            choices += "]";
            QString comboPart = QString(comboField).arg(i).arg(width).arg(checkWidth).arg(choices) + "}";
            QString parameterRow = QString(rowBodyText + comboPart + "}").arg(check).arg(parameters[i]._label).arg(width).arg(i).arg(checkWidth).arg(xshift).arg(visibile);
            formRows += parameterRow;
            if ( results != "")
                results += "+ \"|\" +";
            results += QString("pin_%1.currentText").arg(i);
        }
    }
    if ( !input){
        formRows.replace("pin_","pout_");
        formRows.replace("optionalOutputMarker",";");
//        formRows.replace("optionalOutputMarker",";text : outputfield_0;onTextChanged:{ if( text !== outputfield_0){ outputfield_0=text}}");
    }else {
        formRows.replace("optionalOutputMarker","");
    }
    return formRows;
}

QString ApplicationFormExpressionParser::index2Form(quint64 metaid, bool showoutputformat, bool showEmptyOptionInList, QString invisibleFieldIndexes) const {
    Resource resource = mastercatalog()->id2Resource(metaid);
    std::vector<FormParameter> parameters = getParameters(resource);

    std::vector<FormParameter> outparameters = getOutputParameters(resource);
    QString results;
    QString columnStart = "import QtQuick 2.2; import QtQuick.Controls 1.1;import QtQuick.Layouts 1.1;import MasterCatalogModel 1.0;Column { %1 x:5; width : parent.width - 5; height : parent.height;spacing :10;";
    QString exclusiveGroup = "ExclusiveGroup { id : sourceFilterGroup; onCurrentChanged: {}}";
    columnStart += exclusiveGroup;
    int width = 0;
    for(int i = 0; i < parameters.size(); ++i){
        width = std::max(parameters[i]._label.size(), width);
    }
    width *= 10;
    width = std::min(100, width);

    QString inputpart = makeFormPart(width, parameters, true, results, showEmptyOptionInList, invisibleFieldIndexes);
    QString outputPart;
    QString seperator;
    if ( showoutputformat){
        outputPart = makeFormPart(width, outparameters, false, results, showEmptyOptionInList);
        results = "property var outputFormats;property string formresult : " + results;
        for(int i = 0; i < outparameters.size(); ++i){
            results += QString(";property string outputfield_%1").arg(i);
            if ( hasType(outparameters[i]._dataType, itCOVERAGE | itTABLE)){
                results += QString(";property alias format_%1 :  pout_format_%1").arg(i);
            }
        }
        results += ";";
        seperator = "Rectangle{width : parent.width - 12; x: 6; height:2;color : \"#B3B3B3\"}";
    }else
        results = "property string formresult : " + results + ";";
    columnStart = QString(columnStart).arg(results);
    QString component = columnStart + inputpart + seperator + outputPart + "}";

   //  for debugging, check if the qml is ok; can be retrieved from teh log file
  // kernel()->issues()->log(component);

    return component;

}

QString ApplicationFormExpressionParser::createWorkflowForm(quint64 metaid) const {
    QString columnStart = "import QtQuick 2.2; import QtQuick.Controls 1.1;import QtQuick.Layouts 1.1;import Column { %1 x:5; width : parent.width - 5; height : parent.height;spacing :10;";
    QString exclusiveGroup = "ExclusiveGroup { id : sourceFilterGroup; onCurrentChanged: {}}";
    columnStart += exclusiveGroup;

    std::vector<FormParameter> parameters = createWorkflowMetadata(metaid);

    int width = 0;
    for(int i = 0; i < parameters.size(); ++i){
        width = std::max(parameters[i]._label.size(), width);
    }
    width *= 10;
    width = std::min(100, width);

    QString results;
    QString inputpart = makeFormPart(width, parameters, true, results, false);
    results = "property string formresult : " + results + ";property string outputfield_0;";
    columnStart = QString(columnStart).arg(results);

    //QString seperator = "Rectangle{width : parent.width - 12; x: 6; height:2;color : \"#B3B3B3\"}";

    QString component = columnStart + inputpart + "}";

    MESSAGE1("new dynamic qml component: %1", component);
    return component;
}


std::vector<ApplicationFormExpressionParser::FormParameter> ApplicationFormExpressionParser::createWorkflowMetadata(quint64 metaid) const
{
    std::vector<FormParameter> parameters;

    FormParameter metadataname;
    metadataname._label = "Name";
    metadataname._dataType = itTEXTDOMAIN;
    metadataname._fieldType = ftTEXTEDIT;
    parameters.push_back(metadataname);

    FormParameter metadatadescription;
    metadatadescription._label = "Description";
    metadatadescription._dataType = itTEXTDOMAIN;
    metadatadescription._fieldType = ftTEXTAREA;
    parameters.push_back(metadatadescription);

    return parameters;
}

QString ApplicationFormExpressionParser::formats(const QString& query, quint64 ilwtype) const
{
    if ( hasType(ilwtype,itFEATURE )){
        ilwtype = itFEATURE;
    }

    std::multimap<QString, Ilwis::DataFormat>  formats = Ilwis::DataFormat::getSelectedBy(Ilwis::DataFormat::fpNAME, query);
    QString formatList;
    for(auto &format : formats)    {
        if ( formatList != ""){
            formatList += ",";
        }
        formatList += "'"+ format.second.property(Ilwis::DataFormat::fpNAME).toString() + "'";
    }
    if ( formatList != "")
       formatList = "'Memory'," + formatList;
    if ( hasType(ilwtype, itTABLE)){
        formatList = "'Keep original'," + formatList;
    }
    if ( formatList != "")
        formatList = "[" + formatList + "]";
    return formatList;
}
