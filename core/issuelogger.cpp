#include <QSqlError>
#include "errorobject.h"
#include "issuelogger.h"

using namespace Ilwis;

IssueObject::IssueObject()
{
}

Ilwis::IssueObject::IssueObject(const QString &message, int it)
{
    _message = message;
    _itype = it;
    _itime = QDateTime::currentDateTime();
}

QString Ilwis::IssueObject::message() const
{
    //TODO some control over the output format of this string(should it have time or not?, location etc)
    return _message;
}

QDateTime Ilwis::IssueObject::time() const
{
    return _itime;
}

int Ilwis::IssueObject::type() const
{
    return _itype;
}

QString Ilwis::IssueObject::logMessage(Ilwis::IssueObject::LogMessageFormat) const
{
    QString type = "Unknown";
    if ( _itype == itError)
        type = "Unknown";
    return QString("%1 %2 %3").arg(type, _itime.toString(), _message);
}

//---------------------------------------------------------------------------
IssueLogger::IssueLogger() : _repeatCount(0)
{
}

void IssueLogger::log(const QString &message, int it)
{
    ++_repeatCount;
    if ( _lastmessage == message && _repeatCount == 10) {
        return;
    } else {
        if ( _repeatCount > 10 && _lastmessage != message ){
            _issues.enqueue(IssueObject(QString("Message repeated %1 times").arg(_repeatCount), it));
            throw ErrorObject("Error message cascade");
        }
        _repeatCount = 0;
    }

    _issues.enqueue(IssueObject(message, it));
    _lastmessage = message;
}

void IssueLogger::log(const QString& objectName, const QString &message, int it)
{
    QString newmessage = objectName + ":" + message;
    log(newmessage, it);
}


void IssueLogger::logSql(const QSqlError &err)
{
    //TODO more info about the error for databases
   log(err.text(), IssueObject::itError);
}

QString IssueLogger::popList(int, int) {
    //TODO prints <number> of items of the stack
    return "?";
}

IssueObject::IssueType IssueLogger::maxIssueLevel() const
{
    int type = IssueObject::itNone;
    foreach(IssueObject issue, _issues) {
        type |= issue.type();
    }
    if ( type & IssueObject::itCritical)
        return IssueObject::itCritical;
    if ( type & IssueObject::itError)
        return IssueObject::itError;
    if ( type & IssueObject::itWarning)
        return IssueObject::itWarning;
    if ( type & IssueObject::itMessage)
        return IssueObject::itMessage;
    return IssueObject::itNone;
}

void IssueLogger::copy(IssueLogger &other)
{
  foreach(IssueObject issue, _issues) {
      other._issues.enqueue(issue);
  }
}

QString IssueLogger::popfirst(int) {
    //TODO filtering on issue type
    if ( _issues.size() > 0)
        return _issues.dequeue().message();
    return "?";
}

QString IssueLogger::poplast(int) {
   //TODO filtering on issue type
    if ( _issues.size() >0 )
        return _issues.takeLast().message();
    return "?";
}

void IssueLogger::clear() {
    _issues.clear();
}



