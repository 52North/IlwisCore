#ifndef ERRORMODEL_H
#define ERRORMODEL_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QAbstractListModel>
#include "ilwiscoreui_global.h"


class Message {

public:
    Message(int id, QString msg);
    ~Message();

    int id();
    QString msg();

private:
    int _id;
    QString _msg;
};


class ILWISCOREUISHARED_EXPORT ErrorModel : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QStringList errorlist READ errorlist NOTIFY errorlistChanged)

    static ErrorModel* getInstance();

    ~ErrorModel();

    QStringList errorlist();

    Q_INVOKABLE void addError(int id, QString msg);
    Q_INVOKABLE void clearList();
    Q_INVOKABLE int getCount();


private:
    static bool instanceFlag;
    static ErrorModel *_instance;

    QStringList _errorlist;


signals:
    void errorlistChanged();
};

#endif // ERRORMODEL_H
