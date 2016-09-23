#ifndef CONSOLESCRIPTMODEL_H
#define CONSOLESCRIPTMODEL_H

#include <QObject>
#include <QQmlListProperty>
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT ConsoleScriptModel : public QObject
{
    Q_OBJECT

public:
    explicit ConsoleScriptModel(QObject *parent = 0);
    ~ConsoleScriptModel();

    Q_INVOKABLE QString run(const QString& txt);
    Q_INVOKABLE void countLines(const QString& txt);
    Q_INVOKABLE void findExecutionLine(const QString& txt, int point);
    Q_INVOKABLE int numberOfLines() const;
    Q_INVOKABLE int lastExecutionLine() const;

    bool addLine(const QString& txt);
signals:
    void linesChanged();

public slots:

private:
    int _lastExecutionLine = 0;
    int _numberOfLines = 0;
};

#endif // CONSOLESCRIPTMODEL_H
