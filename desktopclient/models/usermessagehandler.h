#ifndef ILWISERRORHANDLER_H
#define ILWISERRORHANDLER_H

#include <QQmlListProperty>
#include <QColor>
#include "kernel.h"
#include "issuelogger.h"
#include "juliantime.h"

#include <QObject>

class MessageModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString time READ time CONSTANT)
    Q_PROPERTY(QString shorttime READ shorttime CONSTANT)
    Q_PROPERTY(QString message READ message CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(quint64 id READ id CONSTANT)
    Q_PROPERTY(QColor color READ color CONSTANT)

public:
    MessageModel();
    explicit MessageModel(const Ilwis::IssueObject& issue, QObject *parent);
    QString time() const;
    QString shorttime() const;
    QString type() const;
    QString message() const;
    quint64 id();
    QColor color() const;
    QString messageIcon() const;
private:
    Ilwis::IssueObject _message;
};

typedef QQmlListProperty<MessageModel> QMLMessageList;

class UserMessageHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QMLMessageList messages READ messages NOTIFY messageChanged)
    Q_PROPERTY(QString messageIcon READ messageIcon NOTIFY messageIconChanged)

public:
    explicit UserMessageHandler(QObject *parent = 0);

    QMLMessageList messages();
    QString messageIcon() const;

signals:
    void messageChanged();
    void messageIconChanged();

public slots:
    void addMessage(const Ilwis::IssueObject &issue);

private:
   QList<MessageModel *> _messages;

};

Q_DECLARE_METATYPE(QMLMessageList)



#endif // ERRORHANDLER_H
