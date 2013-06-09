#ifndef MODIFIERPART_H
#define MODIFIERPART_H

class DomPartNode;

class ModifierPart : public ASTNode
{
public:
    ModifierPart();

    QString fnamespace() const;
    QString format() const;
};

#endif // MODIFIERPART_H
