#ifndef COLUMN_H
#define COLUMN_H

namespace Ilwis {
class KERNELSHARED_EXPORT ColumnDefinition : public Identity, public ValueDefiner
{
public:
    ColumnDefinition();
    ColumnDefinition(const QString& nm, const IDomain &dom, quint64 colindex = i64UNDEF);
    bool isValid() const;

    QString type() const;

};
}

#endif // COLUMN_H
