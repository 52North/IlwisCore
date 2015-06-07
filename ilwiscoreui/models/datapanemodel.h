#ifndef DATAPANEMODEL_H
#define DATAPANEMODEL_H

#include <QObject>
#include <QQuickItem>

#include "ilwiscoreui_global.h"

class TabModel;

class ILWISCOREUISHARED_EXPORT SidePanelModel : public QObject {
   Q_OBJECT

    Q_PROPERTY(QQuickItem *tabview READ tabview CONSTANT)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(int tabCount READ tabCount CONSTANT)
    Q_PROPERTY(QString side READ side CONSTANT)
public:
   SidePanelModel(QObject *parent = 0);
   SidePanelModel(const QString& side, QObject *parent = 0);

   QQuickItem *tabview() const;
   Q_INVOKABLE void setTabview(const QString& name);
   Q_INVOKABLE TabModel *createPanel(quint32 index, const QString& filter, const QString &outputtype, const QString &url);
   Q_INVOKABLE TabModel *tab(int index);


   QString state() const;
   void setState(const QString& st);
   bool removeTab(int tabIndex);
   int width() const;
   void width(int w);
   void select(int index, bool yesno);
   bool isSelected(int index) const;
   bool isActive() const;
   int tabCount() const;
   QString side() const;

signals:
    void stateChanged();
private:
    QString _tabview;
    std::vector<TabModel *> _tabs;
    QString _state;
    QString _side;

};

class ILWISCOREUISHARED_EXPORT DataPaneModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(SidePanelModel* activeSide READ activeSide NOTIFY onActiveSideChanged)
    Q_PROPERTY(bool leftActive READ leftActive CONSTANT)
    Q_PROPERTY(SidePanelModel* rightSide READ rightSide CONSTANT)
    Q_PROPERTY(SidePanelModel* leftSide READ leftSide CONSTANT)

public:
    explicit DataPaneModel(QObject *parent = 0);
    Q_INVOKABLE bool removeTab(bool lftside, int tabIndex);
    Q_INVOKABLE TabModel *tab(bool lftside, int index);
    Q_INVOKABLE void select(bool lftside, int index, bool yesno);
    Q_INVOKABLE bool isSelected(bool lftside, int index) const;
    bool leftActive() const;

    SidePanelModel* rightSide() const;
    SidePanelModel* leftSide() const;
    Q_INVOKABLE TabModel *createPanel(const QString &filter, const QString &outputtype, const QString &url);

signals:
    void onActiveSideChanged();

public slots:

private:
    SidePanelModel *_leftside = 0;
    SidePanelModel *_rightside = 0;

    SidePanelModel* activeSide() const;
};

#endif // DATAPANEMODEL_H
