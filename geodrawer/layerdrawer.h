#ifndef LAYERDRAWER_H
#define LAYERDRAWER_H

#include <QMatrix4x4>
#include "colorlookup.h"
#include "representation.h"
#include "spatialdatadrawer.h"

class QOpenGLShaderProgram;

namespace Ilwis {
namespace Geodrawer{

class DrawingColor;
typedef std::unique_ptr<DrawingColor> UPDrawingColor;

class LayerDrawer : public SpatialDataDrawer
{
public:
    LayerDrawer(const QString& name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);

    void cleanUp();
    QString activeAttribute() const;

    void setAttribute(const QString &attrName, const QVariant &attrib);
    QVariant attribute(const QString &key) const;

    QColor color(const IRepresentation& rpr, double value, ColorValueMeaning cvm = cvmTRUEVALUE);

protected:
    virtual void setActiveVisualAttribute(const QString& attr);


private:
    QString _activeAttribute;


    bool initShaders();

};


}
}

#endif // LAYERDRAWER_H
