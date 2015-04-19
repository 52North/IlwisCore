#ifndef COLUMNMODEL_H
#define COLUMNMODEL_H

#include <QObject>

class ColumnModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
    Q_PROPERTY(QColor backColor READ backColor WRITE backTextColor NOTIFY backColorChanged)
    Q_PROPERTY(QString headerText READ headerText CONSTANT)
    Q_PROPERTY(QString defaultWidth READ defaultWidth CONSTANT)
    Q_PROPERTY(bool selected READ selected WRITE selected NOTIFY selectedChanged)

public:
    explicit ColumnModel(const QString& header, quint32 defwidth, QObject *parent = 0);
    ~ColumnModel();

    QString textColor() const;
    void setTextColor(const QColor& clr);
    QColor backColor() const;
    void setBackColor(const QColor& clr);
    QString headerText() const;
    quint32 defaultWidth() const;
    bool selected() const;
    void setSelected(bool yesno);

signals:
    void textColorChanged();
    void backColorChanged();
    void selectedChanged();

public slots:

private:
    QColor _textColor;
    QColor _backColor;
    QString _headerText;
    QString _defaultWidth = 60;
    bool _selected = false;

};

#endif // COLUMNMODEL_H
