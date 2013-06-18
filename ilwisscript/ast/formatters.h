#ifndef MODIFIERPART_H
#define MODIFIERPART_H

namespace Ilwis {
class DomPartNode;

class Formatters : public ASTNode
{
public:
    Formatters();

    QString fnamespace() const;
    QString format() const;
};
}

#endif // MODIFIERPART_H
