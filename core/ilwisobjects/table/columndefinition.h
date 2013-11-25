#ifndef COLUMN_H
#define COLUMN_H

namespace Ilwis {
class KERNELSHARED_EXPORT ColumnDefinition : public Identity
{
public:
    ColumnDefinition();
    ColumnDefinition(const ColumnDefinition& def, quint32 index);
    ColumnDefinition(const QString& nm, const IDomain &dom, quint64 colindex = i64UNDEF);
    bool isValid() const;

    QString type() const;
    const DataDefinition &datadef() const;
    DataDefinition &datadef();
    bool isMultiple() const;
    void multiple(bool yesno);
    void columnindex(quint64 idx);
    quint64 columnindex() const;
    bool isChanged() const;
    void changed(bool yesno);
private:
    DataDefinition _datadef;
    bool _multiple;
    bool _changed;

};

KERNELSHARED_EXPORT bool operator==(const ColumnDefinition& def1, const ColumnDefinition& def2);
KERNELSHARED_EXPORT bool operator!=(const ColumnDefinition& def1, const ColumnDefinition& def2);
}

#endif // COLUMN_H
