#ifndef UICONTEXTMODEL_H
#define UICONTEXTMODEL_H

#include <memory>
#include <QObject>
#include <QVariant>
#include "iooptions.h"
#include "ilwiscoreui_global.h"

using namespace Ilwis;

class PropertyEditor;
class ObjectVisualizationModel;

typedef std::function<PropertyEditor *(ObjectVisualizationModel *vismodel, const IOOptions& options)> CreatePropertyEditor;

class ILWISCOREUISHARED_EXPORT UIContextModel : public QObject
{
    Q_OBJECT
public:
    friend std::unique_ptr<UIContextModel>& uicontext();

    explicit UIContextModel(QObject *parent = 0);

    void addPropertyEditor(quint64 objecttype, const QString& propertyName, CreatePropertyEditor createFunction);
    std::map<QString, CreatePropertyEditor> propertyEditors(quint64 objecttype) const;
signals:

public slots:

private:
    void init();
    std::map<quint64,std::map<QString, CreatePropertyEditor>> _propertyEditors;
    static std::unique_ptr<UIContextModel>_uicontext;

};

std::unique_ptr<UIContextModel>& uicontext();


#endif // UICONTEXTMODEL_H
