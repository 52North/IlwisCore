#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "mastercatalog.h"
#include "linestylesetter.h"

using namespace Ilwis;

//REGISTER_PROPERTYEDITOR(itCOVERAGE,"linestyleeditor",LineStyleSetter)


LineStyleSetter::LineStyleSetter(QObject *parent) : PropertyEditor(parent)
{
}

int LineStyleSetter::defaultHeight() const
{
    return 60;
}

PropertyEditor *LineStyleSetter::create()
{
    return new LineStyleSetter();
}
