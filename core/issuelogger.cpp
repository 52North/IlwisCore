#include <QSqlError>
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QDataStream>
#include <fstream>
#include "ilwiscontext.h"
#include "errorobject.h"
#include "issuelogger.h"

using namespace Ilwis;

IssueObject::IssueObject()
{
}

Ilwis::IssueObject::IssueObject(const QString &message, int it, quint64 id)
{
    _message = message;
    _itype = it;
    _itime = QDateTime::currentDateTime();
    _id = id;
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

int IssueObject::codeLine() const {
    return _line;
}

QString IssueObject::codeFunc() const {
    return _func;
}

QString IssueObject::codeFile() const {
    return _file;
}

void IssueObject::addCodeInfo(int line, const QString &func, const QString &file)
{
    _line = line;
    _func = func;
    _file = file;
}

void IssueObject::stream(std::ofstream& stream, LogMessageFormat frmt) {
    stream << _id << " ; " <<_itime.toString().toStdString() << " ; " << _message.toStdString() << std::endl;
    if ( frmt == lmCODE) {
        stream << _id << " ; " << _line << " : " << _func.toStdString() << " ; " << _file.toStdString() << std::endl;
    }
}

quint64 IssueObject::id() const
{
    return _id;
}

//---------------------------------------------------------------------------
IssueLogger::IssueLogger() : _repeatCount(0)
{
    QString apploc= context()->ilwisFolder().absoluteFilePath();
    apploc += "/log";
    QDir dir(apploc);
    if ( !dir.exists())
        dir.mkdir(apploc);
    QString rlogFilePath = apploc + "/logfile.txt";
    QString clogFilePath = apploc + "/logfile_ext.txt";
    _logFileRegular.open(rlogFilePath.toLatin1());
    _logFileCode.open(clogFilePath.toLatin1());
}

IssueLogger::~IssueLogger()
{
    if (_logFileCode.is_open())
        _logFileCode.close();
    if ( _logFileRegular.is_open())
        _logFileRegular.close();
}

quint64 IssueLogger::log(const QString &message, int it)
{
    ++_repeatCount;
    if ( _lastmessage == message && _repeatCount == 10) {
        return _issueId;
    } else {
        if ( _repeatCount > 10 && _lastmessage != message ){
            _issues.enqueue(IssueObject(QString("Message repeated %1 times").arg(_repeatCount), it, _issueId));
            throw ErrorObject("Error message cascade");
        }
        _repeatCount = 0;
    }

    _issues.enqueue(IssueObject(message, it, _issueId));
    if ( _lastmessage == message)
        return _issueId;
    IssueObject& obj = _issues.back();
    if ( _logFileRegular.is_open()) {
        obj.stream(_logFileRegular, IssueObject::lmREGULAR);
    }

    _lastmessage = message;
    return _issueId++;
}

quint64 IssueLogger::log(const QString& objectName, const QString &message, int it)
{
    QString newmessage = objectName + ":" + message;
    return log(newmessage, it);
}

void IssueLogger::addCodeInfo(quint64 issueid, int line, const QString &func, const QString &file)
{
    for(auto iter=_issues.begin(); iter != _issues.end(); ++iter) {
        IssueObject& issue = *iter;
        if ( issue.id() == issueid) {
            issue.addCodeInfo(line, func, file);
            if ( _logFileCode.is_open()) {
                issue.stream(_logFileCode, IssueObject::lmCODE);
            }
            break;
        }
    }
}


quint64 IssueLogger::logSql(const QSqlError &err)
{
    //TODO more info about the error for databases
   return log(err.text(), IssueObject::itError);
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



