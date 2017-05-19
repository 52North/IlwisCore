#ifndef ERRORHANDLING_H
#define ERRORHANDLING_H

#include <QObject>
#include <QDateTime>
#include <QQueue>
#include <fstream>
#include <thread>
#include <map>
#include <mutex>
#include "kernel_global.h"

class QSqlError;

namespace Ilwis {
/*!
 \brief logs issues within the system

 Ilwis error/warning system is a system that stacks issues that arose in a logger in the kernel. Usualy methods that generate
 an error also give a false/invalid return value. It is up to the programmer than to decide what to do. He always has a trace of what
 happened in the issue logger. With creation of objects the issues that arose during creation are transfered to the objects after creation while the general
 stack is cleared.
 The philosophy of the error system is that it stays out of the way unless the client wants is.

*/
class KERNELSHARED_EXPORT IssueObject : public QObject {
    Q_OBJECT
public:
    enum LogMessageFormat{lmFULL, lmREGULAR, lmCODE};
    /*!
     \brief identifies the severity of the issue

     Critical errors will stop the system. Usualy are errors in the configuration of the system
     Errors will not stop the system but probably will abort the action or miss some part of the action
     Warnings are meant to signify non critical changes to the action. Results might be sligthly off but not overly so
     Messages are just messages, no effect on the action
    */
    enum IssueType{itNone=0, itCritical=1, itError=2, itWarning=4, itMessage=8, itDebug=16, itRESULT=32, itExpression=64, itAll=255};

    IssueObject(QObject *parent = 0);
    IssueObject(const QString& message, int it, quint64 id, QObject *parent=0);
    IssueObject(const IssueObject &issueobj);
    IssueObject &operator =(const IssueObject &issueobj);

    QString message() const;
    QDateTime time() const;
    int type() const;
    QString logMessage(LogMessageFormat lmf = lmFULL) const;
    void addCodeInfo(int line, const QString& func, const QString& file);
    quint64 id() const;

    int codeLine() const;
    QString codeFunc() const;
    QString codeFile() const;
    void stream(std::ofstream &stream, LogMessageFormat frmt);


private:
    QString type2String() const;

    QDateTime _itime;
    QString   _message;
    quint64 _id;
    int _line = -1;
    QString _func = "?";
    QString _file = "?";

    int _itype;
};

class KERNELSHARED_EXPORT IssueLogger : public QObject
{
    Q_OBJECT

public:
    explicit IssueLogger(QObject *parent=0);
    ~IssueLogger();

    quint64 log(const QString& message, int it=IssueObject::itError);
    quint64 logSql(const QSqlError& err);
    quint64 log(const QString &objectName, const QString &message, int it=IssueObject::itError);
    void addCodeInfo(quint64 issueid,int line, const QString& func, const QString& file);

    QString popfirst(int tp=IssueObject::itAll);
    QString poplast(int it=IssueObject::itAll);
    IssueObject::IssueType maxIssueLevel() const;
    void copy(QList<IssueObject>& other);
    void clear();
    bool silent() const;
    void silent(bool yesno);

signals:
    void updateIssues(const IssueObject& issue);

private:
    QString _lastmessage;
    quint32 _repeatCount;
    quint64 _issueId=0;
    QQueue<IssueObject> _issues;
    std::ofstream _logFileRegular;
    std::ofstream _logFileCode;
    static std::map<std::thread::id, bool> _silentThreads;
    mutable std::recursive_mutex _guard;


};
}

Q_DECLARE_METATYPE( Ilwis::IssueObject)

#endif // ERRORHANDLING_H
