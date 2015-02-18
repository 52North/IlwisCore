#ifndef SELECTIONDRAWER_H
#define SELECTIONDRAWER_H

#include <QObject>
#include <QVariant>
#include "box.h"
#include "simpledrawer.h"

namespace Ilwis{

namespace Geodrawer{
class SelectionDrawer : public SimpleDrawer
{
public:
    SelectionDrawer(DrawerInterface* parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);
    ~SelectionDrawer();

    bool draw(const IOOptions& options=IOOptions()) ;
    bool prepare(PreparationType prepType, const IOOptions& options);

    void setAttribute(const QString& attrName, const QVariant& attrib);
    bool drawerAttribute(const QString drawername, const QString& attrName, const QVariant& attrib);
    DrawerInterface::DrawerType drawerType() const;
    quint32 defaultOrder() const;
    Envelope  envelope() const;
    void cleanUp();

    static DrawerInterface *create(DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);

    NEW_DRAWER

private:
    QVector<QVector3D> _vertices;
    QVector<QVector3D> _normals;
    std::vector<VertexIndex> _indices;
    std::vector<VertexColor> _colors;
    QMatrix4x4 _view,_projection, _model, _mvp;
    bool _preserveAspectRatio = true;

};
}
}

#endif // SELECTIONDRAWER_H
