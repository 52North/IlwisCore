#ifndef IDNODE_H
#define IDNODE_H

namespace Ilwis {
class IDNode : public ASTNode
{
public:
    IDNode(char *s);
    void setType(int ty);
    QString nodeType() const;
    quint64 type() const;
    QString id() const;

    /*!
     \brief id nodes may represent a 'real' value or a pointer to an entry in the symbol table

     As id's in the script are on a parser level are indistinguishable with respect to being a representation of a existing object
     or an entry in the symbol table, we must be able to distinguish between them to get its value. The isreference member indicates
     if the value is equal to the id or if the value is in the symbol table

      \return bool true if the value can be found in the symbol table
    */
    bool isReference() const;
    bool evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx);

protected:
    IlwisTypes tentativeFileType(const QString &ext) const;

    quint64 _type;
    QString _text;
    bool _isreference;


};
}

#endif // IDNODE_H
