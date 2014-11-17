#ifndef VISUALIZATIONMANAGER_H
#define VISUALIZATIONMANAGER_H

#include <memory>
#include <QObject>

#include "objectvisualizationmodel.h"

class ObjectVisualizationModel;

class VisualizationManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<QString> globalProperties READ globalProperties CONSTANT)
public:
    friend std::unique_ptr<VisualizationManager>& visualizationManager();

    explicit VisualizationManager(QObject *parent = 0);

    std::map<QString, QString> propertyEditors(quint64 objecttype) const;
    void addPropertyEditor(quint64 objecttype, const QString& propertyName, const QString& qmlLink);
    void addVisualizationModel(quint32 visualizatonid, quint32 layerIndex, ObjectVisualizationModel *newmodel);
    QStringList globalProperties() const;

    Q_INVOKABLE ObjectVisualizationModel *layer(quint32 visualizationid, quint32 layerIndex);
    Q_INVOKABLE QMLObjectVisualizationList layersOf(quint32 visualizationid);
signals:
    void removeLayer(const Ilwis::Resource& resource);

public slots:

private:
    std::map<quint64, std::map<QString, QString>> _propertyEditors;
    std::map<quint32, QList<ObjectVisualizationModel *>> _visualizations;
    static std::unique_ptr<VisualizationManager> _manager;
    void init();
    QList<ObjectVisualizationModel *> _empty;
    QList<ObjectVisualizationModel *> _test;

};

std::unique_ptr<VisualizationManager>& visualizationManager();

#endif // VISUALIZATIONMANAGER_H
