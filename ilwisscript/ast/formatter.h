#ifndef FORMATNODE_H
#define FORMATNODE_H

class FormatNode : public ASTNode
{
public:
    FormatNode();

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

#endif // FORMATNODE_H
