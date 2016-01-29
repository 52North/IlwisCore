#include <QString>
#include <queue>
#include "ilwis.h"
#include "iooptions.h"
#include "ilwisobject.h"
#include "catalogquery.h"

using namespace Ilwis;

CatalogQuery::CatalogQuery()
{
    int bits = 1 + log(itILWISOBJECT) / log(2);
    for(int i=1 ;i < bits; ++i) {
        _names[IlwisObject::type2Name(pow(2,i))] = pow(2,i);
    }
    _names["catalog"] = itCATALOG;
    _names["featurecoverage"] = itFEATURE;
    _names["coordinatesystem"] = itCOORDSYSTEM;
    _names["domain"] = itDOMAIN;
    _names["table"] = itTABLE;
    _names["coverage"] = itCOVERAGE;
    _names["representation"] = itREPRESENTATION;
    _names["workflow"] = itWORKFLOW;
}

bool CatalogQuery::checkForProperty(const std::vector<QString>& resourceBaseNames, QString& side, bool left, bool uselike) const
{
    bool ok;
    side = side.trimmed();
    side.toDouble(&ok);
    bool inpropertiestable = true; //= std::find(resourceBaseNames.begin(), resourceBaseNames.end(), side) == resourceBaseNames.end();
    for(auto iter : resourceBaseNames ){
        if ( side.indexOf((iter)) == 0){
            inpropertiestable = false;
            break;
        }
    }
    //    if ( side[0] == '\'' && side[side.size()-1] == '\''){ // just a string
    //        return false;
    //    }
    if ( !ok && inpropertiestable){
        if ( left){
            side = QString("catalogitemproperties.propertyname='%1'").arg(side);
            side += " and ";
        }else {
            if (uselike){
                side = side.replace('\'','%');
                side = "'" + side + "'";
                side = QString("catalogitemproperties.propertyvalue like %1").arg(side);
            }else
                side = QString("catalogitemproperties.propertyvalue=%1").arg(side);
        }

    }
    else if (!ok && side[0] != '\'' && !inpropertiestable){
        side = "mastercatalog." + side;
    }
    return inpropertiestable;

}

QString CatalogQuery::transformQuery(const QString& baseQuery) const{
    if ( baseQuery == "" || baseQuery == sUNDEF)
        return "";

    QString query = baseQuery.toLower();
    for(auto name = _names.constBegin(); name != _names.constEnd(); ++name) {
        QString findTxt = "'" + name.key().toLower() + "'";
        query.replace(findTxt, QString::number(name.value()));
    }
    std::vector<QString> resourceBaseNames={"type", "extendedtype","size","dimensions","url","type&","container","rawcontainer","itemid"};
    QString specialchars{"()<>=!+- "};
    QString leftside, rightside, newquery;
    QString middel;
    std::queue<QString> brackets;
    bool inquotes=false, onleftside = true;
    for(auto c : query){

        if ( (specialchars.indexOf(c) == -1) || inquotes){
            if (onleftside){
                leftside += c;
            }else{
                rightside += c;
            }
        }
        if ( c == ' ' && (leftside == "and" || leftside == "or")){
            newquery += leftside + " ";
            leftside = rightside = middel = "";
            onleftside = true;
        } else if (c == ' ' && !onleftside && !inquotes && rightside != ""){
            bool likecases = leftside == "keyword";
            if( checkForProperty(resourceBaseNames, leftside, true,false))
                middel = "";
            checkForProperty(resourceBaseNames, rightside, false, likecases);

            newquery += leftside + middel + rightside + " ";
            if ( brackets.size() > 0){
                QString br = brackets.front();;
                newquery = br == "(" ? br + newquery : newquery + br;
                brackets.pop();
            }
            leftside = rightside = middel = "";
            onleftside = true;
        }
        if ( c == '\''){
            inquotes = !inquotes;

        }  if (!inquotes){
            if ( specialchars.indexOf(c) != -1 && c != ' '){
                if( c != '(' && c != ')'){
                    middel += c;
                    onleftside = false;
                }else
                    brackets.push(c);
            }
        }

    }
    bool likecases = leftside == "keyword";
    if ( checkForProperty(resourceBaseNames, leftside, true, false))
        middel = "";
    if ( leftside.indexOf("mastercatalog.") != 0)
        checkForProperty(resourceBaseNames, rightside, false, likecases);

    newquery += leftside + middel + rightside;
    if ( brackets.size() > 0){
        QString br = brackets.front();;
        newquery = br == "(" ? br + newquery : newquery + br;
        brackets.pop();
    }

    return newquery;
}
