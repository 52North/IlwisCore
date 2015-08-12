#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "models/tablemodel.h"
#include "tableoperation.h"
#include "sortcolumn.h"

using namespace Ilwis;
using namespace Desktop;

SortColumn::SortColumn() : TableOperation("Column Sort", QUrl("SortColumn.qml"),TR("Sorts columns in ascending or decending order"))
{

}

bool Ilwis::Desktop::SortColumn::canUse(TableModel *tblModel, const QVariantMap& parameter) const
{
    bool ok;
    quint32 colIndex = parameter["columnindex"].toInt(&ok);
    if ( !ok || colIndex >= tblModel->getColumnCount())
        return false;
    IDomain dom = tblModel->table()->columndefinition(colIndex).datadef().domain();

    return hasType(dom->valueType(), itNUMBER | itSTRING | itDOMAINITEM);

}

template<typename T> struct OrderedPair{
    OrderedPair(){}
    OrderedPair(const T& value, quint32 order) : _sortableValue(value), _order(order){}
    T _sortableValue;
    quint32 _order;

};

void SortColumn::execute(const QVariantMap& parameters )
{
    bool ok;
    quint32 colIndex = parameters["columnindex"].toInt(&ok);
    if ( !ok || colIndex >= tableModel()->getColumnCount())
        return ;
    QString direction = parameters["order"].toString();
    if ( direction != "ascending" && direction != "decending")
        return ;

    IlwisTypes valueType = table()->columndefinition(colIndex).datadef().domain()->valueType();
    std::vector<QVariant> values = table()->column(colIndex);
    std::vector<quint32> order(values.size());
    quint32 index = 0;

    if ( hasType(valueType, itNUMBER)){
        std::vector<OrderedPair<double>> numbers(values.size());
        for(auto& opair : numbers){
            opair = OrderedPair<double>(values[index].toDouble(), index);
            index++;
        }
        std::sort(numbers.begin(), numbers.end(),[](const OrderedPair<double>& opair1,const OrderedPair<double>& opair2)->bool { return opair1._sortableValue < opair2._sortableValue;});
        index = 0;
        for(const auto& opair : numbers)
            order[index++] = opair._order;
    }
    if ( hasType(valueType, itSTRING)){
        std::vector<OrderedPair<QString>> strings(values.size());
        for(auto& opair : strings){
            opair = OrderedPair<QString>(values[index].toString(), index);
            index++;
        }
        index = 0;
        std::sort(strings.begin(), strings.end(),[](const OrderedPair<QString>& opair1,const OrderedPair<QString>& opair2)->bool { return opair1._sortableValue < opair2._sortableValue;});
        for(const auto& opair : strings)
            order[index++] = opair._order;
    }
    if ( hasType(valueType, itDOMAINITEM)){
        std::vector<OrderedPair<QString>> strings(values.size());
        IDomain dom = table()->columndefinition(colIndex).datadef().domain();
        for(auto& opair : strings){
            opair = OrderedPair<QString>(dom->impliedValue(values[index]).toString(), index);
            index++;
        }
        index = 0;
        std::sort(strings.begin(), strings.end(),[](const OrderedPair<QString>& opair1,const OrderedPair<QString>& opair2)->bool { return opair1._sortableValue < opair2._sortableValue;});
        for(const auto& opair : strings)
            order[index++] = opair._order;

    }
    tableModel()->order(order);
    tableModel()->update();
}

TableOperation *SortColumn::create()
{
    return new SortColumn();
}
