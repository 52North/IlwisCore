#ifndef UICONTEXTMODEL_H
#define UICONTEXTMODEL_H

#include <QObject>

class UIContextModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentKey READ currentKey WRITE currentKey NOTIFY currentKeyChanged)
    Q_PROPERTY(int modifierKey READ modifierKey CONSTANT)
public:
    explicit UIContextModel(QObject *parent = 0);
    void currentKey(int keycode);
    int currentKey() const;
    int modifierKey() const;


signals:
    void currentKeyChanged();

public slots:

private:
    int _currentKey;
    int _activeModifierKey;

};

};

#endif // UICONTEXTMODEL_H
