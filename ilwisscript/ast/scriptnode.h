#ifndef SCRIPTNODE_H
#define SCRIPTNODE_H



namespace Ilwis {

class Formatter;

class ScriptNode : public ASTNode
{
public:
    ScriptNode();
    QString nodeType() const;
    static Formatter *activeFormat(IlwisTypes type);
    static void setActiveFormat(quint64, const QSharedPointer<ASTNode>& node);

private:
    static std::map<quint64, QSharedPointer<ASTNode> > _activeFormat;
};
}

#endif // SCRIPTNODE_H
