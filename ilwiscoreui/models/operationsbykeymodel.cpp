#include "operationsbykeymodel.h"

OperationsByKeyModel::OperationsByKeyModel()
{

}

OperationsByKeyModel::OperationsByKeyModel(const QString& kword, const std::vector<OperationModel *> &opers, QObject *parent) :
    QObject(parent), _keyword(kword)
{
    for(auto element : opers)
        _operations.push_back(element);
}

QString OperationsByKeyModel::keyword() const
{
    return _keyword;
}

QMLOperationList OperationsByKeyModel::operationsList()
{
    return  QQmlListProperty<OperationModel>(this, _operations);
}

int OperationsByKeyModel::operationCount() const
{
    return _operations.size();
}
