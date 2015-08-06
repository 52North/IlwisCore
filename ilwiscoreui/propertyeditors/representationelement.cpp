#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "representation.h"
#include "representationelement.h"
#include "attributeeditor.h"

RepresentationElement::RepresentationElement(VisualAttributeEditor *p) : QObject(p)
{
}

RepresentationElement::RepresentationElement(const QString &label, VisualAttributeEditor *p) : QObject(p), Identity(label)
{
}

RepresentationElement::RepresentationElement(Raw raw, const QString &label, const QColor &clr, VisualAttributeEditor *parent) : QObject(parent), Identity(label), _color(clr), _raw(raw)
{
}

QColor RepresentationElement::color() const
{
    return _color;
}

void RepresentationElement::color(const QColor &clr)
{
    _color = clr;
    emit ecolorChanged();
}

