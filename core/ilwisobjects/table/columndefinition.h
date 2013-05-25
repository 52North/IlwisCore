#ifndef COLUMN_H
#define COLUMN_H

namespace Ilwis {
class KERNELSHARED_EXPORT ColumnDefinition : public Identity
{
public:
    ColumnDefinition();
    ColumnDefinition(const QString& nm, const IDomain &dom, quint64 colindex = i64UNDEF);
    bool isValid() const;

    QString type() const;
    const DataDefinition &datadef() const;
    DataDefinition &datadef();
private:
    DataDefinition _datadef;

};
}

#endif // COLUMN_H
