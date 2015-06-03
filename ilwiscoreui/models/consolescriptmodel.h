#ifndef CONSOLESCRIPTMODEL_H
#define CONSOLESCRIPTMODEL_H

#include <QObject>
#include <QQmlListProperty>
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT ConsoleLineModel : public QObject{
    Q_OBJECT

    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(int lineNumber READ lineNumber CONSTANT)
    Q_PROPERTY(bool readonly READ readOnly WRITE readOnly CONSTANT)
public:
    ConsoleLineModel(QObject *parent = 0);
    ConsoleLineModel(const QString& txt, bool readonly, QObject *parent = 0);

    QString content() const;
    void setContent(const QString& txt);
    int lineNumber() const;
    void lineNumber(int l);
    void readOnly(bool yesno);
    bool readOnly() const;

signals:
    void contentChanged();

private:
    QString _content;
    int _lineNumber = 0;
    bool _readonly = false;
};
class ILWISCOREUISHARED_EXPORT ConsoleScriptModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<ConsoleLineModel> lines READ lines NOTIFY linesChanged)
    Q_PROPERTY(int numberOfLines READ numberOfLines CONSTANT)
public:
    explicit ConsoleScriptModel(QObject *parent = 0);
    ~ConsoleScriptModel();

    QQmlListProperty<ConsoleLineModel> lines() ;
    Q_INVOKABLE ConsoleLineModel *addLine(const QString& txt, bool readonly);
    Q_INVOKABLE QString run(const QString& txt);
    int numberOfLines() const;

signals:
    void linesChanged();

public slots:

private:
    QList<ConsoleLineModel *> _lines;
};

#endif // CONSOLESCRIPTMODEL_H
