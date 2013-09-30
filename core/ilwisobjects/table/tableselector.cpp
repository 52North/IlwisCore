#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "logicalexpressionparser.h"
#include "connectorinterface.h"
#include "basetable.h"
#include "flattable.h"
#include "tableselector.h"

using namespace Ilwis;

TableSelector::TableSelector()
{
}

std::vector<quint32> TableSelector::select(const Table* table, const QString &conditions)
{
    LogicalExpressionParser parser(conditions);
    if ( !parser.isValid()) {
        return std::vector<quint32>();
    }
    std::vector<bool> status(table->records(),false);
    for(auto part : parser.parts()) {
        std::vector<QVariant> data = table->column(part.field());
        if (data.size() == 0) {
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("expression"), conditions);
            return std::vector<quint32>();
        }
        const ColumnDefinition& coldef = table->columndefinition(part.field());

        auto iter = status.begin();
        IlwisTypes vt = part.valueType();
        for(const QVariant& var : data) {
            if ( hasType(vt, itNUMBER))
                numericCase(part, var.toDouble(), iter);
            else if ( hasType(vt, itSTRING )){
                stringCase(part, coldef, var, iter);
            }

            ++iter;
        }

    }
    std::vector<quint32> result;
    for(int i=0; i < status.size(); ++i)
        if(status[i])
            result.push_back(i);

    return result;
}
void TableSelector::stringCase(const LogicalExpressionPart& part, const ColumnDefinition& coldef, const QVariant& var, std::vector<bool>::iterator& iter) {
    QString text = part.value();
    QString fieldtxt = var.toString();
    if ( coldef.datadef().domain()->valueType() == itTHEMATICITEM){
        quint32 raw = var.toUInt();
        INamedIdDomain domainid = coldef.datadef().domain().get<NamedIdDomain>();
        SPDomainItem item = domainid->item(raw);
        fieldtxt = item->name();
    }
    LogicalOperator lconnector =  part.logicalConnector();
    switch(part.condition()){
    case loEQ:{
        switch(lconnector){
        case loNONE:
            *iter = fieldtxt == text;break;
        case loOR:
            *iter = *iter || fieldtxt == text;break;
        case loAND:
            *iter = *iter && fieldtxt == text;break;
        default:
            *iter = false;break;
        }
        break;
    }
    case loNEQ:{
        switch(lconnector){
        case loNONE:
            *iter = fieldtxt != text;break;
        case loOR:
            *iter = *iter || fieldtxt != text;break;
        case loAND:
            *iter = *iter && fieldtxt != text;break;
        default:
            *iter = false;break;
        }
        break;
    }
    default:
        *iter = false;
    }
}


void TableSelector::numericCase(const LogicalExpressionPart& part, double val1, std::vector<bool>::iterator& iter) {
    double val2 = part.value().toDouble();
    LogicalOperator lconnector =  part.logicalConnector();
    switch(part.condition()){
    case loEQ:{
        switch(lconnector){
        case loNONE:
            *iter = val1 == val2;break;
        case loOR:
            *iter = *iter || val1 == val2;break;
        case loAND:
            *iter = *iter && val1 == val2;break;
        default:
            *iter = false;break;
        }
        break;
    }
    case loNEQ:{
        switch(lconnector){
        case loNONE:
            *iter = val1 != val2;break;
        case loOR:
            *iter = *iter || val1 != val2;break;
        case loAND:
            *iter = *iter && val1 != val2;break;
        default:
            *iter = false;break;
        }
        break;
    }
    case loLESS:{
        switch(lconnector){
        case loNONE:
            *iter = (val1 < val2) && val1 !=rUNDEF && val2 != rUNDEF;break;
        case loOR:
            *iter = (*iter || val1 < val2) && val1 !=rUNDEF && val2 != rUNDEF;break;
        case loAND:
            *iter = (*iter && val1 < val2) && val1 !=rUNDEF && val2 != rUNDEF;break;
        default:
            *iter = false;break;
        }
        break;
    }
    case loLESSEQ:{
        switch(lconnector){
        case loNONE:
            *iter = (val1 <= val2) && val1 !=rUNDEF && val2 != rUNDEF;break;
        case loOR:
            *iter = (*iter || val1 <= val2) && val1 !=rUNDEF && val2 != rUNDEF;break;
        case loAND:
            *iter = (*iter && val1 <= val2) && val1 !=rUNDEF && val2 != rUNDEF;break;
        default:
            *iter = false;break;
        }
        break;
    }
    case loGREATEREQ:{
        switch(lconnector){
        case loNONE:
            *iter = (val1 >= val2) && val1 !=rUNDEF && val2 != rUNDEF;break;
        case loOR:
            *iter = (*iter || val1 >= val2) && val1 !=rUNDEF && val2 != rUNDEF;break;
        case loAND:
            *iter = (*iter && val1 >= val2) && val1 !=rUNDEF && val2 != rUNDEF;break;
        default:
            *iter = false;break;
        }
        break;
    }
    case loGREATER:{
        switch(lconnector){
        case loNONE:
            *iter = (val1 > val2) && val1 !=rUNDEF && val2 != rUNDEF;break;
        case loOR:
            *iter = (*iter || val1 > val2) && val1 !=rUNDEF && val2 != rUNDEF;break;
        case loAND:
            *iter = (*iter && val1 > val2) && val1 !=rUNDEF && val2 != rUNDEF;break;
        default:
            *iter = false;break;
        }
        break;
    }
    default:
        *iter = false;break;

    }
}
