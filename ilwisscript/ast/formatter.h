#ifndef FORMATNODE_H
#define FORMATNODE_H

namespace Ilwis {
class Formatter : public ASTNode
{
public:
    Formatter();

    void setFormatNameSpace(IDNode *node);
    void setFormat(char *s);
    void setDataType(const QString& tp);
    QString fnamespace() const;
    QString format() const;
    QString dataType() const;
private:
    QSharedPointer<IDNode> _namespace;
    QString _format;
    QString _dataType;
};
}

#endif // FORMATNODE_H
