#ifndef BASEDRAWER_H
#define BASEDRAWER_H


#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include "iooptions.h"
#include "drawers/drawerinterface.h"
#include "box.h"
#include "identity.h"

namespace Ilwis {
namespace Geodrawer{

class RootDrawer;

class BaseDrawer : public DrawerInterface, public Ilwis::Identity
{
public:
    enum Containment { cINSIDE, cOUTSIDE, cUNKNOWN};



    RootDrawer* rootDrawer() ;
    const RootDrawer *rootDrawer() const;
    DrawerInterface* parent();
    const DrawerInterface *parent() const;

    bool isActive() const ;
    void active(bool yesno);
    bool isValid() const;
    void selected(bool yesno);
    bool isSelected() const;
    Containment containment() const;
    void cleanUp(QOpenGLContext *openglContext);

    void code(const QString& code);
    QString code() const;
    quint64 id() const;
    QString name() const;
    void name(const QString& n);
    QString description() const;
    void setDescription(const QString& desc);

    std::vector<QVariant> attributes(const QString& attrNames) const;
    QVariant attribute(const QString& attrName) const;
    virtual void setAttribute(const QString&, const QVariant&);
    virtual bool drawerAttribute(const QString layername, const QString& attrName, const QVariant& attrib);

    QColor color(const IRepresentation& rpr,double value, DrawerInterface::ColorValueMeaning cvm = cvmTRUEVALUE);
    quint32 defaultOrder() const;

protected:
    BaseDrawer(const QString &name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);
    void valid(bool yesno);
    bool prepare(DrawerInterface::PreparationType prepType, const IOOptions& opt, QOpenGLContext * ctx=0);
    void unprepare(PreparationType prepType);
    bool isPrepared(quint32 type=ptALL) const;
    bool draw(QOpenGLContext *, const IOOptions&) const;
    const QMatrix4x4 &mvpMatrix() const;
    virtual bool initGeometry(QOpenGLContext *openglContext, const std::vector<VertexPosition> &vertices, const std::vector<VertexColor> &colors);

    quint32 _prepared = 0;

    GLuint _vboPosition = iUNDEF;
    GLuint _vboColor = iUNDEF;
    QOpenGLShaderProgram _shaders;

private:
    bool _active = true; // unless defined otherwise, the drawer is active
    bool _valid = false;
    bool _selected = false;
    RootDrawer* _rootDrawer = 0;
    DrawerInterface* _parentDrawer = 0;
    Envelope _envelope;





};

typedef std::unique_ptr<QOpenGLFunctions> UPOpenGL;
}
}

#endif // BASEDRAWER_H
