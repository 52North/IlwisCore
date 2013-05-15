#include "ilwis.h"
#include "astnode.h"
#include "idnode.h"
#include "valuerangenode.h"
#include "dompartnode.h"
#include "formatnode.h"
#include "modifierpart.h"

ModifierPart::ModifierPart() : ASTNode("modifierpart")
{
}

QString ModifierPart::fnamespace() const
{
    if ( noOfChilderen() == 1) {
        FormatNode *format = static_cast<FormatNode *>(child(0).data());
        return format->fnamespace();
    }
    return "";
}

QString ModifierPart::format() const
{
    if ( noOfChilderen() == 1) {
        FormatNode *format = static_cast<FormatNode *>(child(0).data());
        return format->format();
    }

    return sUNDEF;
}

