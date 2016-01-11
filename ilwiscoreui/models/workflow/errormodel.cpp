#include "errormodel.h"
#include <QString>
#include <QAbstractListModel>
#include <QDebug>

Message::Message(int id, QString msg)
{
    _id = id;
    _msg = msg;
}

Message::~Message()
{
}

int Message::id()
{
    return _id;
}

QString Message::msg()
{
    return _msg;
}


bool ErrorModel::instanceFlag = false;
ErrorModel* ErrorModel::_instance = NULL;
ErrorModel* ErrorModel::getInstance()
{
    if(! instanceFlag)
    {
        _instance = new ErrorModel();
        instanceFlag = true;
        return _instance;
    }
    else
    {
        return _instance;
    }
}

ErrorModel::~ErrorModel()
{
    instanceFlag = false;
}

QStringList ErrorModel::errorlist()
{
    return _errorlist;
}

void ErrorModel::addError(int id, QString msg)
{
    _errorlist.append(msg);
    emit errorlistChanged();
}

void ErrorModel::clearList()
{
    _errorlist.clear();
    emit errorlistChanged();
}
